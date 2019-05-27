#include "cbe_decoder.h"

// #define KSLogger_LocalLevel TRACE
#include "kslogger.h"
#include "test_utils.h"

static cbe_decoder* get_decoder(struct cbe_decode_process* process)
{
    return (cbe_decoder*)cbe_decode_get_user_context(process);
}

static bool on_nil(struct cbe_decode_process* process)
{
    return get_decoder(process)->set_next(enc::nil()) && get_decoder(process)->get_callback_return_value();
}

static bool on_boolean(struct cbe_decode_process* process, bool value)
{
    return get_decoder(process)->set_next(enc::bl(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_int(struct cbe_decode_process* process, int value)
{
    return get_decoder(process)->set_next(enc::si(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_int_64(struct cbe_decode_process* process, int64_t value)
{
    return get_decoder(process)->set_next(enc::si(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_int_128(struct cbe_decode_process* process, int128_ct value)
{
    return get_decoder(process)->set_next(enc::si(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_uint(struct cbe_decode_process* process, unsigned value)
{
    return get_decoder(process)->set_next(enc::ui(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_uint_64(struct cbe_decode_process* process, uint64_t value)
{
    return get_decoder(process)->set_next(enc::ui(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_uint_128(struct cbe_decode_process* process, uint128_ct value)
{
    return get_decoder(process)->set_next(enc::ui(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_float_32(struct cbe_decode_process* process, float value)
{
    return get_decoder(process)->set_next(enc::flt(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_float_64(struct cbe_decode_process* process, double value)
{
    return get_decoder(process)->set_next(enc::flt(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_float_128(struct cbe_decode_process* process, float128_ct value)
{
    return get_decoder(process)->set_next(enc::flt(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_decimal_32(struct cbe_decode_process* process, dec32_ct value)
{
    return get_decoder(process)->set_next(enc::dec(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_decimal_64(struct cbe_decode_process* process, dec64_ct value)
{
    return get_decoder(process)->set_next(enc::dec(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_decimal_128(struct cbe_decode_process* process, dec128_ct value)
{
    return get_decoder(process)->set_next(enc::dec(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_smalltime(struct cbe_decode_process* process, smalltime value)
{
    return get_decoder(process)->set_next(enc::time(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_nanotime(struct cbe_decode_process* process, nanotime value)
{
    return get_decoder(process)->set_next(enc::time(value)) && get_decoder(process)->get_callback_return_value();
}

static bool on_list_begin(struct cbe_decode_process* process)
{
    return get_decoder(process)->set_next(enc::list()) && get_decoder(process)->get_callback_return_value();
}

static bool on_list_end(struct cbe_decode_process* process)
{
    return get_decoder(process)->set_next(enc::end()) && get_decoder(process)->get_callback_return_value();
}

static bool on_map_begin(struct cbe_decode_process* process)
{
    return get_decoder(process)->set_next(enc::map()) && get_decoder(process)->get_callback_return_value();
}

static bool on_map_end(struct cbe_decode_process* process)
{
    return get_decoder(process)->set_next(enc::end()) && get_decoder(process)->get_callback_return_value();
}

static bool on_string_begin(struct cbe_decode_process* process, int64_t byte_count)
{
    return get_decoder(process)->string_begin(byte_count) && get_decoder(process)->get_callback_return_value();
}

static bool on_bytes_begin(struct cbe_decode_process* process, int64_t byte_count)
{
    return get_decoder(process)->bytes_begin(byte_count) && get_decoder(process)->get_callback_return_value();
}

static bool on_uri_begin(struct cbe_decode_process* process, int64_t byte_count)
{
    return get_decoder(process)->uri_begin(byte_count) && get_decoder(process)->get_callback_return_value();
}

static bool on_comment_begin(struct cbe_decode_process* process, int64_t byte_count)
{
    return get_decoder(process)->comment_begin(byte_count) && get_decoder(process)->get_callback_return_value();
}

static bool on_string_data(struct cbe_decode_process* process,
                           const char* start,
                           int64_t byte_count)
{
    return get_decoder(process)->add_string_data(std::string(start, start + byte_count)) && get_decoder(process)->get_callback_return_value();
}

static bool on_bytes_data(struct cbe_decode_process* process,
                           const uint8_t* start,
                           int64_t byte_count)
{
    return get_decoder(process)->add_bytes_data(std::vector<uint8_t>(start, start + byte_count)) && get_decoder(process)->get_callback_return_value();
}

static bool on_uri_data(struct cbe_decode_process* process,
                           const char* start,
                           int64_t byte_count)
{
    return get_decoder(process)->add_uri_data(std::string(start, start + byte_count)) && get_decoder(process)->get_callback_return_value();
}

static bool on_comment_data(struct cbe_decode_process* process,
                           const char* start,
                           int64_t byte_count)
{
    return get_decoder(process)->add_comment_data(std::string(start, start + byte_count)) && get_decoder(process)->get_callback_return_value();
}


ANSI_EXTENSION static const cbe_decode_callbacks g_callbacks =
{
    on_nil: on_nil,
    on_boolean: on_boolean,
    on_int: on_int,
    on_int_64: on_int_64,
    on_int_128: on_int_128,
    on_uint: on_uint,
    on_uint_64: on_uint_64,
    on_uint_128: on_uint_128,
    on_float_32: on_float_32,
    on_float_64: on_float_64,
    on_float_128: on_float_128,
    on_decimal_32: on_decimal_32,
    on_decimal_64: on_decimal_64,
    on_decimal_128: on_decimal_128,
    on_smalltime: on_smalltime,
    on_nanotime: on_nanotime,
    on_list_begin: on_list_begin,
    on_list_end: on_list_end,
    on_map_begin: on_map_begin,
    on_map_end: on_map_end,
    on_string_begin: on_string_begin,
    on_string_data: on_string_data,
    on_bytes_begin: on_bytes_begin,
    on_bytes_data: on_bytes_data,
    on_uri_begin: on_uri_begin,
    on_uri_data: on_uri_data,
    on_comment_begin: on_comment_begin,
    on_comment_data: on_comment_data,
};

cbe_decoder::cbe_decoder(int max_container_depth, bool callback_return_value)
: _process_backing_store(cbe_decode_process_size(max_container_depth))
, _process((cbe_decode_process*)_process_backing_store.data())
, _callback_return_value(callback_return_value)
, _max_container_depth(max_container_depth)
{
}

bool cbe_decoder::get_callback_return_value()
{
    return _callback_return_value;
}

std::vector<uint8_t>& cbe_decoder::received_data()
{
    return _received_data;
}

std::shared_ptr<enc::encoding> cbe_decoder::decoded()
{
    return _decoded;
}

cbe_decode_status cbe_decoder::feed(std::vector<uint8_t>& data)
{
    KSLOG_DEBUG("Feeding %d bytes", data.size());
    KSLOG_TRACE("Feeding %s", as_string(data).c_str());
    _received_data.insert(_received_data.begin(), data.begin(), data.end());
    int64_t byte_count = data.size();
    cbe_decode_status status = cbe_decode_feed(_process, data.data(), &byte_count);
    _read_offset += byte_count;
    return status;
}

cbe_decode_status cbe_decoder::begin()
{
    return cbe_decode_begin(_process, &g_callbacks, (void*)this, _max_container_depth);
}

cbe_decode_status cbe_decoder::end()
{
    if(!_process_is_valid)
    {
        KSLOG_ERROR("Called end() too many times");
        return (cbe_decode_status)9999999;
    }
    _process_is_valid = false;
    return cbe_decode_end(_process);
}

cbe_decode_status cbe_decoder::decode(std::vector<uint8_t>& document)
{
    return cbe_decode(&g_callbacks, (void*)this, document.data(), document.size(), _max_container_depth);
}

bool cbe_decoder::set_next(std::shared_ptr<enc::encoding> encoding)
{
    if(_currently_decoding_type != CBE_DECODING_OTHER)
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

bool cbe_decoder::string_begin(int64_t byte_count)
{
    if(_currently_decoding_type != CBE_DECODING_OTHER)
    {
        KSLOG_ERROR("Expected _currently_decoding_type OTHER, not %d", _currently_decoding_type);
        return false;
    }
    _currently_decoding_type = CBE_DECODING_STRING;
    _currently_decoding_length = byte_count;
    _currently_decoding_offset = 0;
    _currently_decoding_data.clear();
    return true;
}

bool cbe_decoder::bytes_begin(int64_t byte_count)
{
    if(_currently_decoding_type != CBE_DECODING_OTHER)
    {
        KSLOG_ERROR("Expected _currently_decoding_type OTHER, not %d", _currently_decoding_type);
        return false;
    }
    _currently_decoding_type = CBE_DECODING_BYTES;
    _currently_decoding_length = byte_count;
    _currently_decoding_offset = 0;
    _currently_decoding_data.clear();
    return true;
}

bool cbe_decoder::uri_begin(int64_t byte_count)
{
    if(_currently_decoding_type != CBE_DECODING_OTHER)
    {
        KSLOG_ERROR("Expected _currently_decoding_type OTHER, not %d", _currently_decoding_type);
        return false;
    }
    _currently_decoding_type = CBE_DECODING_URI;
    _currently_decoding_length = byte_count;
    _currently_decoding_offset = 0;
    _currently_decoding_data.clear();
    return true;
}

bool cbe_decoder::comment_begin(int64_t byte_count)
{
    if(_currently_decoding_type != CBE_DECODING_OTHER)
    {
        KSLOG_ERROR("Expected _currently_decoding_type OTHER, not %d", _currently_decoding_type);
        return false;
    }
    _currently_decoding_type = CBE_DECODING_COMMENT;
    _currently_decoding_length = byte_count;
    _currently_decoding_offset = 0;
    _currently_decoding_data.clear();
    return true;
}

bool cbe_decoder::add_string_data(const std::string& data)
{
    KSLOG_DEBUG("Add string data %d bytes", data.size());
    if(_currently_decoding_type != CBE_DECODING_STRING)
    {
        KSLOG_ERROR("Expecting _currently_decoding_type STRING, not %d", _currently_decoding_type);
        return false;
    }

    if(_currently_decoding_offset + (int64_t)data.size() > _currently_decoding_length)
    {
        KSLOG_ERROR("_currently_decoding_offset + data.size() (%d) > _currently_decoding_length (%d)",
            _currently_decoding_offset + (int64_t)data.size(), _currently_decoding_length);
        return false;
    }

    _currently_decoding_data.insert(_currently_decoding_data.end(), data.begin(), data.end());
    _currently_decoding_offset += data.size();

    if(_currently_decoding_offset == _currently_decoding_length)
    {
        _currently_decoding_type = CBE_DECODING_OTHER;
        return set_next(enc::str(std::string(_currently_decoding_data.begin(), _currently_decoding_data.end())));
    }

    return true;
}

bool cbe_decoder::add_bytes_data(const std::vector<uint8_t>& data)
{
    KSLOG_DEBUG("Add bytes data %d bytes", data.size());
    if(_currently_decoding_type != CBE_DECODING_BYTES)
    {
        KSLOG_ERROR("Expecting _currently_decoding_type BYTES, not %d", _currently_decoding_type);
        return false;
    }

    if(_currently_decoding_offset + (int64_t)data.size() > _currently_decoding_length)
    {
        KSLOG_ERROR("_currently_decoding_offset + data.size() (%d) > _currently_decoding_length (%d)",
            _currently_decoding_offset + (int64_t)data.size(), _currently_decoding_length);
        return false;
    }

    _currently_decoding_data.insert(_currently_decoding_data.end(), data.begin(), data.end());
    _currently_decoding_offset += data.size();

    if(_currently_decoding_offset == _currently_decoding_length)
    {
        _currently_decoding_type = CBE_DECODING_OTHER;
        return set_next(enc::bin(_currently_decoding_data));
    }

    return true;
}

bool cbe_decoder::add_uri_data(const std::string& data)
{
    KSLOG_DEBUG("Add URI data %d bytes", data.size());
    if(_currently_decoding_type != CBE_DECODING_URI)
    {
        KSLOG_ERROR("Expecting _currently_decoding_type URI, not %d", _currently_decoding_type);
        return false;
    }

    if(_currently_decoding_offset + (int64_t)data.size() > _currently_decoding_length)
    {
        KSLOG_ERROR("_currently_decoding_offset + data.size() (%d) > _currently_decoding_length (%d)",
            _currently_decoding_offset + (int64_t)data.size(), _currently_decoding_length);
        return false;
    }

    _currently_decoding_data.insert(_currently_decoding_data.end(), data.begin(), data.end());
    _currently_decoding_offset += data.size();

    if(_currently_decoding_offset == _currently_decoding_length)
    {
        _currently_decoding_type = CBE_DECODING_OTHER;
        return set_next(enc::uri(std::string(_currently_decoding_data.begin(), _currently_decoding_data.end())));
    }

    return true;
}

bool cbe_decoder::add_comment_data(const std::string& data)
{
    KSLOG_DEBUG("Add comment data %d bytes", data.size());
    if(_currently_decoding_type != CBE_DECODING_COMMENT)
    {
        KSLOG_ERROR("Expecting _currently_decoding_type COMMENT, not %d", _currently_decoding_type);
        return false;
    }

    if(_currently_decoding_offset + (int64_t)data.size() > _currently_decoding_length)
    {
        KSLOG_ERROR("_currently_decoding_offset + data.size() (%d) > _currently_decoding_length (%d)",
            _currently_decoding_offset + (int64_t)data.size(), _currently_decoding_length);
        return false;
    }

    _currently_decoding_data.insert(_currently_decoding_data.end(), data.begin(), data.end());
    _currently_decoding_offset += data.size();

    if(_currently_decoding_offset == _currently_decoding_length)
    {
        _currently_decoding_type = CBE_DECODING_OTHER;
        return set_next(enc::cmt(std::string(_currently_decoding_data.begin(), _currently_decoding_data.end())));
    }

    return true;
}
