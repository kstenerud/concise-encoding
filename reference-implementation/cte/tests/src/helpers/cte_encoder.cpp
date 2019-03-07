#include "cte_encoder.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"

bool cte_encoder::flush_buffer()
{
    bool result = false;
    int64_t offset = cte_encode_get_buffer_offset(_process);
    KSLOG_DEBUG("Flushing %d bytes and resetting buffer to %d bytes", offset, _buffer.size());
    if(offset > 0)
    {
        _encoded_data.insert(_encoded_data.end(), _buffer.begin(), _buffer.begin() + offset);
        result = _on_data_ready(_buffer.data(), offset);
    }
    if(cte_encode_set_buffer(_process, _buffer.data(), _buffer.size()) != CTE_ENCODE_STATUS_OK)
    {
        KSLOG_ERROR("Error setting encode buffer");
        return false;
    }

    return result;
}

cte_encode_status cte_encoder::flush_and_retry(std::function<cte_encode_status()> my_function)
{
    cte_encode_status status = my_function();
    if(status == CTE_ENCODE_STATUS_NEED_MORE_ROOM)
    {
        flush_buffer();
        status = my_function();
    }
    return status;
}


cte_encode_status cte_encoder::encode(enc::number_encoding<int8_t>& e)
{
    return flush_and_retry([&]
    {
        return cte_encode_add_int_8(_process, e.value());
    });
}
cte_encode_status cte_encoder::encode(enc::number_encoding<int16_t>& e)
{
    return flush_and_retry([&]
    {
        return cte_encode_add_int_16(_process, e.value());
    });
}
cte_encode_status cte_encoder::encode(enc::number_encoding<int32_t>& e)
{
    return flush_and_retry([&]
    {
        return cte_encode_add_int_32(_process, e.value());
    });
}
cte_encode_status cte_encoder::encode(enc::number_encoding<int64_t>& e)
{
    return flush_and_retry([&]
    {
        return cte_encode_add_int_64(_process, e.value());
    });
}
cte_encode_status cte_encoder::encode(enc::int128_encoding& e)
{
    return flush_and_retry([&]
    {
        return cte_encode_add_int_128(_process, e.value());
    });
}
cte_encode_status cte_encoder::encode(enc::float_encoding<float>& e)
{
    return flush_and_retry([&]
    {
        return cte_encode_add_float_32(_process, e.get_precision(), e.value());
    });
}
cte_encode_status cte_encoder::encode(enc::float_encoding<double>& e)
{
    return flush_and_retry([&]
    {
        return cte_encode_add_float_64(_process, e.get_precision(), e.value());
    });
}
cte_encode_status cte_encoder::encode(enc::float_encoding<__float128>& e)
{
    return flush_and_retry([&]
    {
        return cte_encode_add_float_128(_process, e.get_precision(), e.value());
    });
}
cte_encode_status cte_encoder::encode(enc::dfp_encoding<_Decimal32>& e)
{
    return flush_and_retry([&]
    {
        return cte_encode_add_decimal_32(_process, e.value());
    });
}
cte_encode_status cte_encoder::encode(enc::dfp_encoding<_Decimal64>& e)
{
    return flush_and_retry([&]
    {
        return cte_encode_add_decimal_64(_process, e.value());
    });
}
cte_encode_status cte_encoder::encode(enc::dfp_encoding<_Decimal128>& e)
{
    return flush_and_retry([&]
    {
        return cte_encode_add_decimal_128(_process, e.value());
    });
}
cte_encode_status cte_encoder::encode(enc::boolean_encoding& e)
{
    return flush_and_retry([&]
    {
        return cte_encode_add_boolean(_process, e.value());
    });
}
cte_encode_status cte_encoder::encode(enc::time_encoding& e)
{
    return flush_and_retry([&]
    {
        return cte_encode_add_time(_process, e.value());
    });
}
cte_encode_status cte_encoder::encode(enc::nil_encoding& e)
{
    (void)e;
    return flush_and_retry([&]
    {
        return cte_encode_add_nil(_process);
    });
}
cte_encode_status cte_encoder::encode(enc::list_encoding& e)
{
    (void)e;
    return flush_and_retry([&]
    {
        return cte_encode_list_begin(_process);
    });
}
cte_encode_status cte_encoder::encode(enc::map_encoding& e)
{
    (void)e;
    return flush_and_retry([&]
    {
        return cte_encode_map_begin(_process);
    });
}

cte_encode_status cte_encoder::stream_array(const std::vector<uint8_t>& data)
{
    const uint8_t* data_pointer = data.data();
    const int64_t total_byte_count = data.size();
    const uint8_t* const data_end = data_pointer + total_byte_count;
    KSLOG_DEBUG("Streaming %d bytes", total_byte_count);

    cte_encode_status status = CTE_ENCODE_STATUS_OK;

    while(data_pointer <= data_end)
    {
        int64_t byte_count = data_end - data_pointer;
        status = cte_encode_add_data(_process, data_pointer, &byte_count);
        if(status != CTE_ENCODE_STATUS_NEED_MORE_ROOM)
        {
            break;
        }
        KSLOG_DEBUG("Streamed %d bytes", byte_count);
        flush_buffer();
        data_pointer += byte_count;
    }

    KSLOG_DEBUG("Done");
    return status;
}

cte_encode_status cte_encoder::encode(enc::string_encoding& e)
{
    const std::string& value = e.value();
    KSLOG_DEBUG("size %d", value.size());
    cte_encode_status status = flush_and_retry([&]
    {
        return cte_encode_string_begin(_process);
    });
    if(status != CTE_ENCODE_STATUS_OK)
    {
        return status;
    }

    status = stream_array(std::vector<uint8_t>(value.begin(), value.end()));
    if(status != CTE_ENCODE_STATUS_OK)
    {
        return status;
    }

    return cte_encode_string_end(_process);
}

cte_encode_status cte_encoder::encode(enc::binary_encoding& e)
{
    KSLOG_DEBUG("size %d", e.value().size());
    cte_encode_status status = flush_and_retry([&]
    {
        return cte_encode_binary_begin(_process);
    });
    if(status != CTE_ENCODE_STATUS_OK)
    {
        return status;
    }

    status = stream_array(e.value());
    if(status != CTE_ENCODE_STATUS_OK)
    {
        return status;
    }

    return cte_encode_binary_end(_process);
}

cte_encode_status cte_encoder::encode(enc::comment_encoding& e)
{
    const std::string& value = e.value();
    KSLOG_DEBUG("size %d", value.size());
    cte_encode_status status = flush_and_retry([&]
    {
        return cte_encode_comment_begin(_process);
    });
    if(status != CTE_ENCODE_STATUS_OK)
    {
        return status;
    }

    status = stream_array(std::vector<uint8_t>(value.begin(), value.end()));
    if(status != CTE_ENCODE_STATUS_OK)
    {
        return status;
    }

    return cte_encode_comment_end(_process);
}

cte_encode_status cte_encoder::encode_string(std::vector<uint8_t> value)
{
    KSLOG_DEBUG("size %d", value.size());
    return flush_and_retry([&]
    {
        return cte_encode_add_string(_process, (const char*)value.data(), value.size());
    });
}

cte_encode_status cte_encoder::encode_comment(std::vector<uint8_t> value)
{
    KSLOG_DEBUG("size %d", value.size());
    return flush_and_retry([&]
    {
        return cte_encode_add_comment(_process, (const char*)value.data(), value.size());
    });
}

cte_encode_status cte_encoder::encode_binary(std::vector<uint8_t> value)
{
    KSLOG_DEBUG("size %d", value.size());
    return flush_and_retry([&]
    {
        return cte_encode_add_binary(_process, value.data(), value.size());
    });
}

cte_encode_status cte_encoder::encode(enc::string_begin_encoding& e)
{
    (void)e;
    return flush_and_retry([&]
    {
        return cte_encode_string_begin(_process);
    });
}

cte_encode_status cte_encoder::encode(enc::binary_begin_encoding& e)
{
    (void)e;
    return flush_and_retry([&]
    {
        return cte_encode_binary_begin(_process);
    });
}

cte_encode_status cte_encoder::encode(enc::comment_begin_encoding& e)
{
    (void)e;
    return flush_and_retry([&]
    {
        return cte_encode_comment_begin(_process);
    });
}

cte_encode_status cte_encoder::encode(enc::string_end_encoding& e)
{
    (void)e;
    return flush_and_retry([&]
    {
        return cte_encode_string_end(_process);
    });
}

cte_encode_status cte_encoder::encode(enc::binary_end_encoding& e)
{
    (void)e;
    return flush_and_retry([&]
    {
        return cte_encode_binary_end(_process);
    });
}

cte_encode_status cte_encoder::encode(enc::comment_end_encoding& e)
{
    (void)e;
    return flush_and_retry([&]
    {
        return cte_encode_comment_end(_process);
    });
}

cte_encode_status cte_encoder::encode(enc::data_encoding& e)
{
    (void)e;
    return stream_array(e.value());
}

cte_encode_status cte_encoder::encode(enc::container_end_encoding& e)
{
    (void)e;
    return flush_and_retry([&]
    {
        return cte_encode_container_end(_process);
    });
}

cte_encode_status cte_encoder::encode(std::shared_ptr<enc::encoding> enc)
{
    cte_encode_status result = cte_encode_begin(_process,
                                                _buffer.data(),
                                                _buffer.size(),
                                                _max_container_depth,
                                                _indent_spaces);
    if(result != CTE_ENCODE_STATUS_OK)
    {
        return result;
    }

    for(std::shared_ptr<enc::encoding> current = enc; current != nullptr; current = current->next())
    {
        result = current->encode(*this);
        if(result != CTE_ENCODE_STATUS_OK)
        {
            flush_buffer();
            return result;
        }
    }

    flush_buffer();
    return cte_encode_end(_process);
}

int64_t cte_encoder::get_encode_buffer_offset()
{
    return cte_encode_get_buffer_offset(_process);
}

cte_encoder::cte_encoder(int64_t buffer_size,
                         int max_container_depth,
                         int indent_spaces,
                         std::function<bool(uint8_t* data_start, int64_t length)> on_data_ready)
: _process_backing_store(cte_encode_process_size(max_container_depth))
, _process((cte_encode_process*)_process_backing_store.data())
, _buffer(buffer_size)
, _max_container_depth(max_container_depth)
, _indent_spaces(indent_spaces)
, _on_data_ready(on_data_ready)
{
    KSLOG_DEBUG("New cte_encoder with buffer size %d", _buffer.size());
}
