#include "cbe_decoder.h"

static cbe_decoder* get_decoder(struct cbe_decode_process* process)
{
    return (cbe_decoder*)cbe_decode_get_user_context(process);
}

static bool on_add_empty(struct cbe_decode_process* process)
{
    return get_decoder(process)->set_next(::empty());
}

static bool on_add_boolean(struct cbe_decode_process* process, bool value)
{
    return get_decoder(process)->set_next(::bl(value));
}

static bool on_add_int_8(struct cbe_decode_process* process, int8_t value)
{
    return get_decoder(process)->set_next(::i8(value));
}

static bool on_add_int_16(struct cbe_decode_process* process, int16_t value)
{
    return get_decoder(process)->set_next(::i16(value));
}

static bool on_add_int_32(struct cbe_decode_process* process, int32_t value)
{
    return get_decoder(process)->set_next(::i32(value));
}

static bool on_add_int_64(struct cbe_decode_process* process, int64_t value)
{
    return get_decoder(process)->set_next(::i64(value));
}

static bool on_add_int_128(struct cbe_decode_process* process, __int128 value)
{
    return get_decoder(process)->set_next(::i128(value));
}

static bool on_add_float_32(struct cbe_decode_process* process, float value)
{
    return get_decoder(process)->set_next(::f32(value));
}

static bool on_add_float_64(struct cbe_decode_process* process, double value)
{
    return get_decoder(process)->set_next(::f64(value));
}

static bool on_add_float_128(struct cbe_decode_process* process, __float128 value)
{
    return get_decoder(process)->set_next(::f128(value));
}

static bool on_add_decimal_32(struct cbe_decode_process* process, _Decimal32 value)
{
    return get_decoder(process)->set_next(::d32(value));
}

static bool on_add_decimal_64(struct cbe_decode_process* process, _Decimal64 value)
{
    return get_decoder(process)->set_next(::d64(value));
}

static bool on_add_decimal_128(struct cbe_decode_process* process, _Decimal128 value)
{
    return get_decoder(process)->set_next(::d128(value));
}

static bool on_add_time(struct cbe_decode_process* process, smalltime value)
{
    return get_decoder(process)->set_next(::time(value));
}

static bool on_begin_list(struct cbe_decode_process* process)
{
    return get_decoder(process)->set_next(::list());
}

static bool on_begin_map(struct cbe_decode_process* process)
{
    return get_decoder(process)->set_next(::map());
}

static bool on_end_container(struct cbe_decode_process* process)
{
    return get_decoder(process)->set_next(::end());
}

static bool on_begin_string(struct cbe_decode_process* process, int64_t byte_count)
{
    return get_decoder(process)->begin_binary(byte_count);
}

static bool on_begin_binary(struct cbe_decode_process* process, int64_t byte_count)
{
    return get_decoder(process)->begin_string(byte_count);
}

static bool on_add_data(struct cbe_decode_process* process,
                     const uint8_t* start,
                     int64_t byte_count)
{
    return get_decoder(process)->add_data(std::vector<uint8_t>(start, start + byte_count));
}


static cbe_decode_callbacks g_callbacks =
{
    on_add_empty: on_add_empty,
    on_add_boolean: on_add_boolean,
    on_add_int_8: on_add_int_8,
    on_add_int_16: on_add_int_16,
    on_add_int_32: on_add_int_32,
    on_add_int_64: on_add_int_64,
    on_add_int_128: on_add_int_128,
    on_add_float_32: on_add_float_32,
    on_add_float_64: on_add_float_64,
    on_add_float_128: on_add_float_128,
    on_add_decimal_32: on_add_decimal_32,
    on_add_decimal_64: on_add_decimal_64,
    on_add_decimal_128: on_add_decimal_128,
    on_add_time: on_add_time,
    on_begin_list: on_begin_list,
    on_begin_map: on_begin_map,
    on_end_container: on_end_container,
    on_begin_string: on_begin_string,
    on_begin_binary: on_begin_binary,
    on_add_data: on_add_data,
};

cbe_decoder::cbe_decoder()
: _process(cbe_decode_begin(&g_callbacks, (void*)this))
{
}

cbe_decode_status cbe_decoder::feed(std::vector<uint8_t>& data)
{
    _received_data.insert(_received_data.begin(), data.begin(), data.end());
    cbe_decode_status status = cbe_decode_feed(_process,
        _received_data.data() + _read_offset,
        _received_data.size() - _read_offset);
    _read_offset += cbe_decode_get_buffer_offset(_process);
    return status;
}

bool cbe_decoder::set_next(std::shared_ptr<encoding> encoding)
{
    if(_currently_decoding_type != CBE_DECODING_OTHER)
    {
        // TODO: Error
        return false;
    }
    _decoded->set_next(encoding);
    return true;
}

bool cbe_decoder::begin_string(int64_t byte_count)
{
    if(_currently_decoding_type != CBE_DECODING_OTHER)
    {
        // TODO: Error
        return false;
    }
    _currently_decoding_type = CBE_DECODING_STRING;
    _currently_decoding_length = byte_count;
    _currently_decoding_offset = 0;
    _currently_decoding_data.clear();
    return true;
}

bool cbe_decoder::begin_binary(int64_t byte_count)
{
    if(_currently_decoding_type != CBE_DECODING_OTHER)
    {
        // TODO: Error
        return false;
    }
    _currently_decoding_type = CBE_DECODING_BINARY;
    _currently_decoding_length = byte_count;
    _currently_decoding_offset = 0;
    _currently_decoding_data.clear();
    return true;
}

bool cbe_decoder::add_data(const std::vector<uint8_t>& data)
{
    if(_currently_decoding_type == CBE_DECODING_OTHER)
    {
        // TODO: Error
        return false;
    }

    if(_currently_decoding_offset + (int64_t)data.size() > _currently_decoding_length)
    {
        // TODO: Error
        return false;
    }

    _currently_decoding_data.insert(_currently_decoding_data.end(), data.begin(), data.end());
    _currently_decoding_offset += data.size();

    if(_currently_decoding_offset == _currently_decoding_length)
    {
        _currently_decoding_type = CBE_DECODING_OTHER;
    }

    return true;
}
