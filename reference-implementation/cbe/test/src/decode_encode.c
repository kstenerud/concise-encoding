#include <stdlib.h>
#include <cbe/cbe.h>
#include "decode_encode.h"

static cbe_encode_context* g_buffer;

static bool on_error(void* context, const char* message)
{
    // TODO: Store this somewhere for the tests to pick up?
    return false;
}

static bool on_empty(void* context)
{
    return cbe_add_empty(g_buffer);
}

static bool on_boolean(void* context, bool value)
{
    return cbe_add_boolean(g_buffer, value);
}

#define DEFINE_ADD_PRIMITIVE(TYPE, NAME_FRAGMENT) \
static bool on_ ## NAME_FRAGMENT(void* context, TYPE value) \
{ \
    cbe_add_ ## NAME_FRAGMENT(g_buffer, value); \
}

DEFINE_ADD_PRIMITIVE(int8_t,     int_8)
DEFINE_ADD_PRIMITIVE(int16_t,    int_16)
DEFINE_ADD_PRIMITIVE(int32_t,    int_32)
DEFINE_ADD_PRIMITIVE(int64_t,    int_64)
DEFINE_ADD_PRIMITIVE(__int128,   int_128)
DEFINE_ADD_PRIMITIVE(float,      float_32)
DEFINE_ADD_PRIMITIVE(double,     float_64)
DEFINE_ADD_PRIMITIVE(__float128, float_128)
DEFINE_ADD_PRIMITIVE(_Decimal32, decimal_32)
DEFINE_ADD_PRIMITIVE(_Decimal64, decimal_64)
DEFINE_ADD_PRIMITIVE(_Decimal128, decimal_128)

bool on_time(void* context, int64_t value)
{
    return cbe_add_time(g_buffer, value);
}

bool on_end_container(void* context)
{
    return cbe_end_container(g_buffer);
}

bool on_list_start(void* context)
{
    return cbe_start_list(g_buffer);
}

bool on_map_start(void* context)
{
    return cbe_start_map(g_buffer);
}

bool on_bitfield(void* context, const uint8_t* start, const uint64_t length)
{
    return cbe_add_bitfield(g_buffer, start, length);
}

bool on_string(void* context, const char* start, const char* end)
{
    return cbe_add_substring(g_buffer, start, end);
}

bool on_array_int_8(void* context, const int8_t* start, const int8_t* end)
{
    return cbe_add_array_int_8(g_buffer, start, end);
}

bool on_array_int_16(void* context, const int16_t* start, const int16_t* end)
{
    return cbe_add_array_int_16(g_buffer, start, end);
}

bool on_array_int_32(void* context, const int32_t* start, const int32_t* end)
{
    return cbe_add_array_int_32(g_buffer, start, end);
}

bool on_array_int_64(void* context, const int64_t* start, const int64_t* end)
{
    return cbe_add_array_int_64(g_buffer, start, end);
}

bool on_array_int_128(void* context, const __int128* start, const __int128* end)
{
    return cbe_add_array_int_128(g_buffer, start, end);
}

bool on_array_float_32(void* context, const float* start, const float* end)
{
    return cbe_add_array_float_32(g_buffer, start, end);
}

bool on_array_float_64(void* context, const double* start, const double* end)
{
    return cbe_add_array_float_64(g_buffer, start, end);
}

bool on_array_float_128(void* context, const __float128* start, const __float128* end)
{
    return cbe_add_array_float_128(g_buffer, start, end);
}

bool on_array_decimal_32(void* context, const _Decimal32* start, const _Decimal32* end)
{
    return cbe_add_array_decimal_32(g_buffer, start, end);
}

bool on_array_decimal_64(void* context, const _Decimal64* start, const _Decimal64* end)
{
    return cbe_add_array_decimal_64(g_buffer, start, end);
}

bool on_array_decimal_128(void* context, const _Decimal128* start, const _Decimal128* end)
{
    return cbe_add_array_decimal_128(g_buffer, start, end);
}

bool on_array_time(void* context, const int64_t* start, const int64_t* end)
{
    // TODO
    // return cbe_add_array_time(g_buffer, start, end);
}

bool decode_encode(const uint8_t* src, int src_length, cbe_encode_context* dst_buffer)
{
    cbe_decode_callbacks callbacks;
    callbacks.on_error = on_error;
    callbacks.on_empty = on_empty;
    callbacks.on_boolean = on_boolean;
    callbacks.on_int_8 = on_int_8;
    callbacks.on_int_16 = on_int_16;
    callbacks.on_int_32 = on_int_32;
    callbacks.on_int_64 = on_int_64;
    callbacks.on_int_128 = on_int_128;
    callbacks.on_float_32 = on_float_32;
    callbacks.on_float_64 = on_float_64;
    callbacks.on_float_128 = on_float_128;
    callbacks.on_decimal_32 = on_decimal_32;
    callbacks.on_decimal_64 = on_decimal_64;
    callbacks.on_decimal_128 = on_decimal_128;
    callbacks.on_time = on_time;
    callbacks.on_end_container = on_end_container;
    callbacks.on_list_start = on_list_start;
    callbacks.on_map_start = on_map_start;
    callbacks.on_bitfield = on_bitfield;
    callbacks.on_string = on_string;
    callbacks.on_array_int_8 = on_array_int_8;
    callbacks.on_array_int_16 = on_array_int_16;
    callbacks.on_array_int_32 = on_array_int_32;
    callbacks.on_array_int_64 = on_array_int_64;
    callbacks.on_array_int_128 = on_array_int_128;
    callbacks.on_array_float_32 = on_array_float_32;
    callbacks.on_array_float_64 = on_array_float_64;
    callbacks.on_array_float_128 = on_array_float_128;
    callbacks.on_array_decimal_32 = on_array_decimal_32;
    callbacks.on_array_decimal_64 = on_array_decimal_64;
    callbacks.on_array_decimal_128 = on_array_decimal_128;
    callbacks.on_array_time = on_array_time;

    g_buffer = dst_buffer;
    return cbe_decode(&callbacks, src, src + src_length, NULL) != NULL;
}
