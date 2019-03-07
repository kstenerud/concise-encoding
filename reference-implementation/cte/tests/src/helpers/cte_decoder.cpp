#include "cte_decoder.h"

// #define KSLogger_LocalLevel TRACE
#include "kslogger.h"
#include "test_utils.h"

static cte_decoder* get_decoder(struct cte_decode_process* process)
{
    return (cte_decoder*)cte_decode_get_user_context(process);
}

static bool on_nil(struct cte_decode_process* process)
{
    return get_decoder(process)->set_next(enc::nil()) && get_decoder(process)->get_callback_return_value();
}

static bool on_boolean(struct cte_decode_process* process, bool value)
{
    return get_decoder(process)->set_next(enc::bl(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_int_8(struct cte_decode_process* process, int8_t value)
{
    return get_decoder(process)->set_next(enc::i8(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_int_16(struct cte_decode_process* process, int16_t value)
{
    return get_decoder(process)->set_next(enc::i16(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_int_32(struct cte_decode_process* process, int32_t value)
{
    return get_decoder(process)->set_next(enc::i32(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_int_64(struct cte_decode_process* process, int64_t value)
{
    return get_decoder(process)->set_next(enc::i64(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_int_128(struct cte_decode_process* process, __int128 value)
{
    return get_decoder(process)->set_next(enc::i128(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_float_32(struct cte_decode_process* process, float value)
{
    return get_decoder(process)->set_next(enc::f32(15, value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_float_64(struct cte_decode_process* process, double value)
{
    return get_decoder(process)->set_next(enc::f64(15, value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_float_128(struct cte_decode_process* process, __float128 value)
{
    return get_decoder(process)->set_next(enc::f128(15, value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_decimal_32(struct cte_decode_process* process, _Decimal32 value)
{
    return get_decoder(process)->set_next(enc::d32(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_decimal_64(struct cte_decode_process* process, _Decimal64 value)
{
    return get_decoder(process)->set_next(enc::d64(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_decimal_128(struct cte_decode_process* process, _Decimal128 value)
{
    return get_decoder(process)->set_next(enc::d128(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_time(struct cte_decode_process* process, smalltime value)
{
    return get_decoder(process)->set_next(enc::smtime(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_list_begin(struct cte_decode_process* process)
{
    return get_decoder(process)->set_next(enc::list()) && get_decoder(process)->get_callback_return_value();
}

static bool on_list_end(struct cte_decode_process* process)
{
    return get_decoder(process)->set_next(enc::end()) && get_decoder(process)->get_callback_return_value();
}

static bool on_map_begin(struct cte_decode_process* process)
{
    return get_decoder(process)->set_next(enc::map()) && get_decoder(process)->get_callback_return_value();
}

static bool on_map_end(struct cte_decode_process* process)
{
    return get_decoder(process)->set_next(enc::end()) && get_decoder(process)->get_callback_return_value();
}

static bool on_string_begin(struct cte_decode_process* process)
{
    return get_decoder(process)->string_begin() && get_decoder(process)->get_callback_return_value();
}

static bool on_binary_begin(struct cte_decode_process* process)
{
    return get_decoder(process)->binary_begin() && get_decoder(process)->get_callback_return_value();
}

static bool on_comment_begin(struct cte_decode_process* process)
{
    return get_decoder(process)->comment_begin() && get_decoder(process)->get_callback_return_value();
}

static bool on_string_data(struct cte_decode_process* process,
                           const char* start,
                           int64_t byte_count)
{
    return get_decoder(process)->add_string_data(std::string(start, start + byte_count)) && get_decoder(process)->get_callback_return_value();
}

static bool on_binary_data(struct cte_decode_process* process,
                           const uint8_t* start,
                           int64_t byte_count)
{
    return get_decoder(process)->add_binary_data(std::vector<uint8_t>(start, start + byte_count)) && get_decoder(process)->get_callback_return_value();
}

static bool on_comment_data(struct cte_decode_process* process,
                           const char* start,
                           int64_t byte_count)
{
    return get_decoder(process)->add_comment_data(std::string(start, start + byte_count)) && get_decoder(process)->get_callback_return_value();
}

static bool on_string_end(struct cte_decode_process* process)
{
    return get_decoder(process)->string_end() && get_decoder(process)->get_callback_return_value();
}

static bool on_binary_end(struct cte_decode_process* process)
{
    return get_decoder(process)->binary_end() && get_decoder(process)->get_callback_return_value();
}

static bool on_comment_end(struct cte_decode_process* process)
{
    return get_decoder(process)->comment_end() && get_decoder(process)->get_callback_return_value();
}


static const cte_decode_callbacks g_callbacks =
{
    on_nil: on_nil,
    on_boolean: on_boolean,
    on_int_8: on_int_8,
    on_int_16: on_int_16,
    on_int_32: on_int_32,
    on_int_64: on_int_64,
    on_int_128: on_int_128,
    on_float_32: on_float_32,
    on_float_64: on_float_64,
    on_float_128: on_float_128,
    on_decimal_32: on_decimal_32,
    on_decimal_64: on_decimal_64,
    on_decimal_128: on_decimal_128,
    on_time: on_time,
    on_list_begin: on_list_begin,
    on_list_end: on_list_end,
    on_map_begin: on_map_begin,
    on_map_end: on_map_end,
    on_string_begin: on_string_begin,
    on_string_data: on_string_data,
    on_string_end: on_string_end,
    on_binary_begin: on_binary_begin,
    on_binary_data: on_binary_data,
    on_binary_end: on_binary_end,
    on_comment_begin: on_comment_begin,
    on_comment_data: on_comment_data,
    on_comment_end: on_comment_end,
};

cte_decoder::cte_decoder(int max_container_depth, bool callback_return_value)
: _process_backing_store(cte_decode_process_size(max_container_depth))
, _process((cte_decode_process*)_process_backing_store.data())
, _callback_return_value(callback_return_value)
, _max_container_depth(max_container_depth)
{
}

bool cte_decoder::get_callback_return_value()
{
    return _callback_return_value;
}

std::vector<uint8_t>& cte_decoder::received_data()
{
    return _received_data;
}

std::shared_ptr<enc::encoding> cte_decoder::decoded()
{
    return _decoded;
}

cte_decode_status cte_decoder::feed(std::vector<uint8_t>& data)
{
    KSLOG_DEBUG("Feeding %d bytes", data.size());
    KSLOG_TRACE("Feeding %s", as_string(data).c_str());
    _received_data.insert(_received_data.begin(), data.begin(), data.end());
    int64_t byte_count = data.size();
    cte_decode_status status = cte_decode_feed(_process, (const char*)data.data(), &byte_count);
    return status;
}

cte_decode_status cte_decoder::begin()
{
    return cte_decode_begin(_process, &g_callbacks, (void*)this, _max_container_depth);
}

cte_decode_status cte_decoder::end()
{
    if(!_process_is_valid)
    {
        KSLOG_ERROR("Called end() too many times");
        return (cte_decode_status)9999999;
    }
    _process_is_valid = false;
    return cte_decode_end(_process);
}

cte_decode_status cte_decoder::decode(std::vector<uint8_t>& document)
{
    return cte_decode(&g_callbacks, (void*)this, (const char*)document.data(), document.size(), _max_container_depth);
}

bool cte_decoder::set_next(std::shared_ptr<enc::encoding> encoding)
{
    if(_currently_decoding_type != CTE_DECODING_OTHER)
    {
        KSLOG_ERROR("Expected _currently_decoding_type OTHER, not %d", _currently_decoding_type);
        return false;
    }
    if(_decoded)
    {
        _decoded->set_next(encoding);
    }
    else
    {
        _decoded = encoding;
    }
    return true;
}

bool cte_decoder::string_begin()
{
    if(_currently_decoding_type != CTE_DECODING_OTHER)
    {
        KSLOG_ERROR("Expected _currently_decoding_type OTHER, not %d", _currently_decoding_type);
        return false;
    }
    _currently_decoding_type = CTE_DECODING_STRING;
    _currently_decoding_data.clear();
    return true;
}

bool cte_decoder::binary_begin()
{
    if(_currently_decoding_type != CTE_DECODING_OTHER)
    {
        KSLOG_ERROR("Expected _currently_decoding_type OTHER, not %d", _currently_decoding_type);
        return false;
    }
    _currently_decoding_type = CTE_DECODING_BINARY;
    _currently_decoding_data.clear();
    return true;
}

bool cte_decoder::comment_begin()
{
    if(_currently_decoding_type != CTE_DECODING_OTHER)
    {
        KSLOG_ERROR("Expected _currently_decoding_type OTHER, not %d", _currently_decoding_type);
        return false;
    }
    _currently_decoding_type = CTE_DECODING_COMMENT;
    _currently_decoding_data.clear();
    return true;
}

bool cte_decoder::add_string_data(const std::string& data)
{
    KSLOG_DEBUG("Add string data %d bytes", data.size());
    if(_currently_decoding_type != CTE_DECODING_STRING)
    {
        KSLOG_ERROR("Expecting _currently_decoding_type STRING, not %d", _currently_decoding_type);
        return false;
    }

    _currently_decoding_data.insert(_currently_decoding_data.end(), data.begin(), data.end());

    return true;
}

bool cte_decoder::string_end()
{
    if(_currently_decoding_type != CTE_DECODING_STRING)
    {
        KSLOG_ERROR("Expecting _currently_decoding_type STRING, not %d", _currently_decoding_type);
        return false;
    }

    _currently_decoding_type = CTE_DECODING_OTHER;
    return set_next(enc::str(std::string(_currently_decoding_data.begin(), _currently_decoding_data.end())));
}

bool cte_decoder::add_binary_data(const std::vector<uint8_t>& data)
{
    KSLOG_DEBUG("Add binary data %d bytes", data.size());
    if(_currently_decoding_type != CTE_DECODING_BINARY)
    {
        KSLOG_ERROR("Expecting _currently_decoding_type BINARY, not %d", _currently_decoding_type);
        return false;
    }

    _currently_decoding_data.insert(_currently_decoding_data.end(), data.begin(), data.end());

    return true;
}

bool cte_decoder::binary_end()
{
    if(_currently_decoding_type != CTE_DECODING_BINARY)
    {
        KSLOG_ERROR("Expecting _currently_decoding_type BINARY, not %d", _currently_decoding_type);
        return false;
    }

    _currently_decoding_type = CTE_DECODING_OTHER;
    return set_next(enc::bin(CTE_BINARY_ENCODING_RADIX_85, _currently_decoding_data));
}

bool cte_decoder::add_comment_data(const std::string& data)
{
    KSLOG_DEBUG("Add comment data %d bytes", data.size());
    if(_currently_decoding_type != CTE_DECODING_COMMENT)
    {
        KSLOG_ERROR("Expecting _currently_decoding_type STRING, not %d", _currently_decoding_type);
        return false;
    }

    _currently_decoding_data.insert(_currently_decoding_data.end(), data.begin(), data.end());

    return true;
}

bool cte_decoder::comment_end()
{
    if(_currently_decoding_type != CTE_DECODING_COMMENT)
    {
        KSLOG_ERROR("Expecting _currently_decoding_type STRING, not %d", _currently_decoding_type);
        return false;
    }

    _currently_decoding_type = CTE_DECODING_OTHER;
    return set_next(enc::comment(std::string(_currently_decoding_data.begin(), _currently_decoding_data.end())));
}
