#include "cbe_encoder.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"

#define MAX_CONTAINER_DEPTH 500

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


cbe_encode_status cbe_encoder::encode(enc::number_encoding<int8_t>& e)
{
	return flush_and_retry([&]
	{
		return cbe_encode_add_int_8(_process, e.value());
	});
}
cbe_encode_status cbe_encoder::encode(enc::number_encoding<int16_t>& e)
{
	return flush_and_retry([&]
	{
		return cbe_encode_add_int_16(_process, e.value());
	});
}
cbe_encode_status cbe_encoder::encode(enc::number_encoding<int32_t>& e)
{
	return flush_and_retry([&]
	{
		return cbe_encode_add_int_32(_process, e.value());
	});
}
cbe_encode_status cbe_encoder::encode(enc::number_encoding<int64_t>& e)
{
	return flush_and_retry([&]
	{
		return cbe_encode_add_int_64(_process, e.value());
	});
}
cbe_encode_status cbe_encoder::encode(enc::int128_encoding& e)
{
	return flush_and_retry([&]
	{
		return cbe_encode_add_int_128(_process, e.value());
	});
}
cbe_encode_status cbe_encoder::encode(enc::number_encoding<float>& e)
{
	return flush_and_retry([&]
	{
		return cbe_encode_add_float_32(_process, e.value());
	});
}
cbe_encode_status cbe_encoder::encode(enc::number_encoding<double>& e)
{
	return flush_and_retry([&]
	{
		return cbe_encode_add_float_64(_process, e.value());
	});
}
cbe_encode_status cbe_encoder::encode(enc::number_encoding<__float128>& e)
{
	return flush_and_retry([&]
	{
		return cbe_encode_add_float_128(_process, e.value());
	});
}
cbe_encode_status cbe_encoder::encode(enc::dfp_encoding<_Decimal32>& e)
{
	return flush_and_retry([&]
	{
		return cbe_encode_add_decimal_32(_process, e.value());
	});
}
cbe_encode_status cbe_encoder::encode(enc::dfp_encoding<_Decimal64>& e)
{
	return flush_and_retry([&]
	{
		return cbe_encode_add_decimal_64(_process, e.value());
	});
}
cbe_encode_status cbe_encoder::encode(enc::dfp_encoding<_Decimal128>& e)
{
	return flush_and_retry([&]
	{
		return cbe_encode_add_decimal_128(_process, e.value());
	});
}
cbe_encode_status cbe_encoder::encode(enc::boolean_encoding& e)
{
	return flush_and_retry([&]
	{
		return cbe_encode_add_boolean(_process, e.value());
	});
}
cbe_encode_status cbe_encoder::encode(enc::time_encoding& e)
{
	return flush_and_retry([&]
	{
		return cbe_encode_add_time(_process, e.value());
	});
}
cbe_encode_status cbe_encoder::encode(enc::nil_encoding& e)
{
	(void)e;
	return flush_and_retry([&]
	{
		return cbe_encode_add_nil(_process);
	});
}
cbe_encode_status cbe_encoder::encode(enc::list_encoding& e)
{
	(void)e;
	return flush_and_retry([&]
	{
		return cbe_encode_list_begin(_process);
	});
}
cbe_encode_status cbe_encoder::encode(enc::map_encoding& e)
{
	(void)e;
	return flush_and_retry([&]
	{
		return cbe_encode_map_begin(_process);
	});
}
cbe_encode_status cbe_encoder::encode(enc::padding_encoding& e)
{
	return flush_and_retry([&]
	{
		return cbe_encode_add_padding(_process, e.byte_count());
	});
}

// Workaround for bizarre vector behavior that produces null pointers on 0 length
static const uint8_t g_dummy_pointer[] = {0};

cbe_encode_status cbe_encoder::stream_array(const std::vector<uint8_t>& data)
{
	int64_t offset = 0;
	KSLOG_DEBUG("Streaming %d bytes", data.size());
	cbe_encode_status status = CBE_ENCODE_STATUS_OK;
	const uint8_t* data_pointer = data.data();
	if(data_pointer == NULL)
	{
		if(data.size() != 0)
		{
			KSLOG_ERROR("data.data() is null!");
			return (cbe_encode_status)99999999;
		}
		data_pointer = g_dummy_pointer;
	}
	while((status = cbe_encode_add_data(_process, data_pointer+offset, data.size()-offset)) == CBE_ENCODE_STATUS_NEED_MORE_ROOM)
	{
		int64_t old_offset = offset;
		(void)old_offset;
		offset = cbe_encode_get_array_offset(_process);
		KSLOG_DEBUG("Streamed %d bytes", offset - old_offset);
		flush_buffer();
		int64_t remaining_bytes = data.size() - offset;
		KSLOG_DEBUG("Remaining bytes to stream: %d", remaining_bytes);
		if(remaining_bytes <= 0)
		{
			break;
		}
	}
	return status;
}

cbe_encode_status cbe_encoder::encode(enc::string_encoding& e)
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

cbe_encode_status cbe_encoder::encode(enc::binary_encoding& e)
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

cbe_encode_status cbe_encoder::encode(enc::comment_encoding& e)
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

cbe_encode_status cbe_encoder::encode(enc::string_header_encoding& e)
{
	return flush_and_retry([&]
	{
		return cbe_encode_string_begin(_process, e.byte_count());
	});
}

cbe_encode_status cbe_encoder::encode(enc::binary_header_encoding& e)
{
	return flush_and_retry([&]
	{
		return cbe_encode_string_begin(_process, e.byte_count());
	});
}

cbe_encode_status cbe_encoder::encode(enc::comment_header_encoding& e)
{
	return flush_and_retry([&]
	{
		return cbe_encode_comment_begin(_process, e.byte_count());
	});
}

cbe_encode_status cbe_encoder::encode(enc::data_encoding& e)
{
    return stream_array(e.value());
}

cbe_encode_status cbe_encoder::encode(enc::container_end_encoding& e)
{
	(void)e;
	return flush_and_retry([&]
	{
		return cbe_encode_container_end(_process);
	});
}

cbe_encode_status cbe_encoder::encode(std::shared_ptr<enc::encoding> enc)
{
	cbe_encode_status result = cbe_encode_begin(_process, _buffer.data(), _buffer.size(), MAX_CONTAINER_DEPTH);
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

cbe_encoder::cbe_encoder(int64_t buffer_size,
	std::function<bool(uint8_t* data_start, int64_t length)> on_data_ready)
: _process_backing_store(cbe_encode_process_size(MAX_CONTAINER_DEPTH))
, _process((cbe_encode_process*)_process_backing_store.data())
, _buffer(buffer_size)
, _on_data_ready(on_data_ready)
{
	KSLOG_DEBUG("New cbe_encoder with buffer size %d", _buffer.size());
}
