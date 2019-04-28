#include "cbe_encoder.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"

bool cbe_encoder::flush_buffer()
{
    bool result = false;
    int64_t offset = cbe_encode_get_buffer_offset(_process);
    KSLOG_DEBUG("Flushing %d bytes and resetting buffer to %d bytes", offset, _buffer.size());
    if(offset > 0)
    {
        _encoded_data.insert(_encoded_data.end(), _buffer.begin(), _buffer.begin() + offset);
        result = _on_data_ready(_buffer.data(), offset);
    }
    if(cbe_encode_set_buffer(_process, _buffer.data(), _buffer.size()) != CBE_ENCODE_STATUS_OK)
    {
        KSLOG_ERROR("Error setting encode buffer");
        return false;
    }

    return result;
}

cbe_encode_status cbe_encoder::flush_and_retry(std::function<cbe_encode_status()> my_function)
{
    cbe_encode_status status = my_function();
    if(status == CBE_ENCODE_STATUS_NEED_MORE_ROOM)
    {
        flush_buffer();
        status = my_function();
    }
    return status;
}


cbe_encode_status cbe_encoder::encode(const enc::int_encoding& e)
{
    return flush_and_retry([&]
    {
        return cbe_encode_add_int_128(_process, e.value());
    });
}
cbe_encode_status cbe_encoder::encode(const enc::uint_encoding& e)
{
    return flush_and_retry([&]
    {
        return cbe_encode_add_uint_128(_process, e.value());
    });
}
cbe_encode_status cbe_encoder::encode(const enc::float_encoding& e)
{
    return flush_and_retry([&]
    {
        return cbe_encode_add_float_128(_process, e.value());
    });
}
cbe_encode_status cbe_encoder::encode(const enc::decimal_encoding& e)
{
    return flush_and_retry([&]
    {
        return cbe_encode_add_decimal_128(_process, e.value());
    });
}
cbe_encode_status cbe_encoder::encode(const enc::boolean_encoding& e)
{
    return flush_and_retry([&]
    {
        return cbe_encode_add_boolean(_process, e.value());
    });
}
cbe_encode_status cbe_encoder::encode(const enc::time_encoding& e)
{
    return flush_and_retry([&]
    {
        return cbe_encode_add_time(_process, e.value());
    });
}
cbe_encode_status cbe_encoder::encode(const enc::nil_encoding& e)
{
    (void)e;
    return flush_and_retry([&]
    {
        return cbe_encode_add_nil(_process);
    });
}
cbe_encode_status cbe_encoder::encode(const enc::list_encoding& e)
{
    (void)e;
    return flush_and_retry([&]
    {
        return cbe_encode_list_begin(_process);
    });
}
cbe_encode_status cbe_encoder::encode(const enc::map_encoding& e)
{
    (void)e;
    return flush_and_retry([&]
    {
        return cbe_encode_map_begin(_process);
    });
}
cbe_encode_status cbe_encoder::encode(const enc::padding_encoding& e)
{
    return flush_and_retry([&]
    {
        return cbe_encode_add_padding(_process, e.byte_count());
    });
}

cbe_encode_status cbe_encoder::stream_array(const std::vector<uint8_t>& data)
{
    const uint8_t* data_pointer = data.data();
    const int64_t total_byte_count = data.size();
    const uint8_t* const data_end = data_pointer + total_byte_count;
    KSLOG_DEBUG("Streaming %d bytes", total_byte_count);

    cbe_encode_status status = CBE_ENCODE_STATUS_OK;

    while(data_pointer <= data_end)
    {
        int64_t byte_count = data_end - data_pointer;
        status = cbe_encode_add_data(_process, data_pointer, &byte_count);
        if(status != CBE_ENCODE_STATUS_NEED_MORE_ROOM)
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

cbe_encode_status cbe_encoder::encode(const enc::string_encoding& e)
{
    const std::string& value = e.value();
    KSLOG_DEBUG("size %d", value.size());
    cbe_encode_status status = flush_and_retry([&]
    {
        return cbe_encode_string_begin(_process, value.size());
    });

    if(status != CBE_ENCODE_STATUS_OK)
    {
        return status;
    }

    return stream_array(std::vector<uint8_t>(value.begin(), value.end()));
}

cbe_encode_status cbe_encoder::encode(const enc::binary_encoding& e)
{
    KSLOG_DEBUG("size %d", e.value().size());
    cbe_encode_status status = flush_and_retry([&]
    {
        return cbe_encode_binary_begin(_process, e.value().size());
    });

    if(status != CBE_ENCODE_STATUS_OK)
    {
        return status;
    }

    return stream_array(e.value());
}

cbe_encode_status cbe_encoder::encode(const enc::comment_encoding& e)
{
    const std::string& value = e.value();
    KSLOG_DEBUG("size %d", value.size());
    cbe_encode_status status = flush_and_retry([&]
    {
        return cbe_encode_comment_begin(_process, value.size());
    });

    if(status != CBE_ENCODE_STATUS_OK)
    {
        return status;
    }

    return stream_array(std::vector<uint8_t>(value.begin(), value.end()));
}

cbe_encode_status cbe_encoder::encode_string(std::vector<uint8_t> value)
{
    KSLOG_DEBUG("size %d", value.size());
    return flush_and_retry([&]
    {
        return cbe_encode_add_string(_process, (const char*)value.data(), value.size());
    });
}

cbe_encode_status cbe_encoder::encode_comment(std::vector<uint8_t> value)
{
    KSLOG_DEBUG("size %d", value.size());
    return flush_and_retry([&]
    {
        return cbe_encode_add_comment(_process, (const char*)value.data(), value.size());
    });
}

cbe_encode_status cbe_encoder::encode_binary(std::vector<uint8_t> value)
{
    KSLOG_DEBUG("size %d", value.size());
    return flush_and_retry([&]
    {
        return cbe_encode_add_binary(_process, value.data(), value.size());
    });
}


cbe_encode_status cbe_encoder::encode(const enc::string_header_encoding& e)
{
    return flush_and_retry([&]
    {
        return cbe_encode_string_begin(_process, e.byte_count());
    });
}

cbe_encode_status cbe_encoder::encode(const enc::binary_header_encoding& e)
{
    return flush_and_retry([&]
    {
        return cbe_encode_binary_begin(_process, e.byte_count());
    });
}

cbe_encode_status cbe_encoder::encode(const enc::comment_header_encoding& e)
{
    return flush_and_retry([&]
    {
        return cbe_encode_comment_begin(_process, e.byte_count());
    });
}

cbe_encode_status cbe_encoder::encode(const enc::data_encoding& e)
{
    return stream_array(e.value());
}

cbe_encode_status cbe_encoder::encode(const enc::container_end_encoding& e)
{
    (void)e;
    return flush_and_retry([&]
    {
        return cbe_encode_container_end(_process);
    });
}

cbe_encode_status cbe_encoder::encode(std::shared_ptr<enc::encoding> enc)
{
    cbe_encode_status result = cbe_encode_begin(_process, _buffer.data(), _buffer.size(), _max_container_depth);
    if(result != CBE_ENCODE_STATUS_OK)
    {
        return result;
    }

    for(std::shared_ptr<enc::encoding> current = enc; current != nullptr; current = current->next())
    {
        result = current->encode(*this);
        if(result != CBE_ENCODE_STATUS_OK)
        {
            flush_buffer();
            return result;
        }
    }

    flush_buffer();
    return cbe_encode_end(_process);
}

int64_t cbe_encoder::get_encode_buffer_offset() const
{
    return cbe_encode_get_buffer_offset(_process);
}

cbe_encoder::cbe_encoder(int64_t buffer_size,
                         int max_container_depth,
                         std::function<bool(uint8_t* data_start, int64_t length)> on_data_ready)
: _process_backing_store(cbe_encode_process_size(max_container_depth))
, _process((cbe_encode_process*)_process_backing_store.data())
, _buffer(buffer_size)
, _max_container_depth(max_container_depth)
, _on_data_ready(on_data_ready)
{
    KSLOG_DEBUG("New cbe_encoder with buffer size %d", _buffer.size());
}
