#include "cbe_encoder.h"

bool cbe_encoder::flush_buffer()
{
	bool result = false;
	int64_t offset = cbe_encode_get_buffer_offset(_process);
	if(offset > 0)
	{
		_encoded_data.insert(_encoded_data.end(), _buffer.begin(), _buffer.begin() + offset);
		result = _on_data_ready(_buffer.data(), offset);
	}
	cbe_encode_set_buffer(_process, _buffer.data(), _buffer.size());

	return result;
}

cbe_encode_status cbe_encoder::encode(enc::number_encoding<int8_t>& e)     {return cbe_encode_add_int_8(_process, e.value());}
cbe_encode_status cbe_encoder::encode(enc::number_encoding<int16_t>& e)    {return cbe_encode_add_int_16(_process, e.value());}
cbe_encode_status cbe_encoder::encode(enc::number_encoding<int32_t>& e)    {return cbe_encode_add_int_32(_process, e.value());}
cbe_encode_status cbe_encoder::encode(enc::number_encoding<int64_t>& e)    {return cbe_encode_add_int_64(_process, e.value());}
cbe_encode_status cbe_encoder::encode(enc::int128_encoding& e)             {return cbe_encode_add_int_128(_process, e.value());}
cbe_encode_status cbe_encoder::encode(enc::number_encoding<float>& e)      {return cbe_encode_add_float_32(_process, e.value());}
cbe_encode_status cbe_encoder::encode(enc::number_encoding<double>& e)     {return cbe_encode_add_float_64(_process, e.value());}
cbe_encode_status cbe_encoder::encode(enc::number_encoding<__float128>& e) {return cbe_encode_add_float_128(_process, e.value());}
cbe_encode_status cbe_encoder::encode(enc::dfp_encoding<_Decimal32>& e)    {return cbe_encode_add_decimal_32(_process, e.value());}
cbe_encode_status cbe_encoder::encode(enc::dfp_encoding<_Decimal64>& e)    {return cbe_encode_add_decimal_64(_process, e.value());}
cbe_encode_status cbe_encoder::encode(enc::dfp_encoding<_Decimal128>& e)   {return cbe_encode_add_decimal_128(_process, e.value());}
cbe_encode_status cbe_encoder::encode(enc::boolean_encoding& e)            {return cbe_encode_add_boolean(_process, e.value());}
cbe_encode_status cbe_encoder::encode(enc::time_encoding& e)               {return cbe_encode_add_time(_process, e.value());}
cbe_encode_status cbe_encoder::encode(enc::empty_encoding& e)              {(void)e; return cbe_encode_add_empty(_process);}
cbe_encode_status cbe_encoder::encode(enc::list_encoding& e)               {(void)e; return cbe_encode_begin_list(_process);}
cbe_encode_status cbe_encoder::encode(enc::map_encoding& e)                {(void)e; return cbe_encode_begin_map(_process);}
cbe_encode_status cbe_encoder::encode(enc::padding_encoding& e)            {return cbe_encode_add_padding(_process, e.byte_count());}

cbe_encode_status cbe_encoder::stream_array(const std::vector<uint8_t>& data)
{
	int64_t offset = 0;
	cbe_encode_status status = CBE_ENCODE_STATUS_OK;
	while((status = cbe_encode_add_data(_process, data.data()+offset, data.size()-offset)) == CBE_ENCODE_STATUS_NEED_MORE_ROOM)
	{
		offset += cbe_encode_get_buffer_offset(_process);
		flush_buffer();
	}
	return status;
}

cbe_encode_status cbe_encoder::encode(enc::string_encoding& e)
{
    cbe_encode_status status;
    if((status = cbe_encode_begin_string(_process, e.value().size())) != CBE_ENCODE_STATUS_OK) return status;
    return stream_array(std::vector<uint8_t>(e.value().begin(), e.value().end()));
}

cbe_encode_status cbe_encoder::encode(enc::binary_encoding& e)
{
    cbe_encode_status status;
    if((status = cbe_encode_begin_binary(_process, e.value().size())) != CBE_ENCODE_STATUS_OK) return status;
    return stream_array(e.value());
}

cbe_encode_status cbe_encoder::encode(enc::end_container_encoding& e)
{
	(void)e;
	if(_process_has_ended)
	{
		// Don't let cbe_encode_end_container() execute twice, because
		// it would double-free memory.
		return (cbe_encode_status)99999999;
	}
	_process_has_ended = true;
	return cbe_encode_end_container(_process);
}

cbe_encode_status cbe_encoder::encode(std::shared_ptr<enc::encoding> enc)
{
	cbe_encode_status result = CBE_ENCODE_STATUS_OK;
	for(std::shared_ptr<enc::encoding> current = enc; current != nullptr; current = current->next())
	{
		result = current->encode(*this);
		if(result != CBE_ENCODE_STATUS_OK)
		{
			return result;
		}
	}
	flush_buffer();
	return result;
}

cbe_encoder::cbe_encoder(int64_t buffer_size,
	std::function<bool(uint8_t* data_start, int64_t length)> on_data_ready)
: _buffer(buffer_size)
, _process(cbe_encode_begin(_buffer.data(), _buffer.size()))
, _on_data_ready(on_data_ready)
{
}

cbe_encoder::~cbe_encoder()
{
	if(!_process_has_ended)
	{
		cbe_encode_end(_process);
	}
}
