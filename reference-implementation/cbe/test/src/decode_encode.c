#include <stdlib.h>
#include <cbe/cbe.h>
#include <memory.h>
#include "decode_encode.h"

static cbe_encode_context* g_encode_context;

static bool on_error(cbe_decode_context* context, const char* message)
{
    // TODO: Store this somewhere for the tests to pick up?
    return false;
}

static bool on_empty(cbe_decode_context* context)
{
    return cbe_encode_add_empty(g_encode_context);
}

static bool on_boolean(cbe_decode_context* context, bool value)
{
    return cbe_encode_add_boolean(g_encode_context, value);
}

#define DEFINE_ADD_PRIMITIVE(TYPE, NAME_FRAGMENT) \
static bool on_ ## NAME_FRAGMENT(cbe_decode_context* context, TYPE value) \
{ \
    cbe_encode_add_ ## NAME_FRAGMENT(g_encode_context, value); \
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

bool on_time(cbe_decode_context* context, int64_t value)
{
    return cbe_encode_add_time(g_encode_context, value);
}

bool on_end_container(cbe_decode_context* context)
{
    return cbe_end_container(g_encode_context);
}

bool on_list_start(cbe_decode_context* context)
{
    return cbe_encode_start_list(g_encode_context);
}

bool on_map_start(cbe_decode_context* context)
{
    return cbe_encode_start_map(g_encode_context);
}

bool on_bitfield(cbe_decode_context* context, const uint8_t* start, const uint64_t length)
{
    return cbe_encode_add_bitfield(g_encode_context, start, length);
}

bool on_string(cbe_decode_context* context, const char* start, const char* end)
{
    return cbe_encode_add_substring(g_encode_context, start, end);
}

bool on_array(cbe_decode_context* context, cbe_data_type data_type, const void* start, uint64_t element_count)
{
    #define HANDLE_ARRAY(NAME, TYPE) \
    { \
        void* copy = malloc(element_count * sizeof(TYPE)); \
        memcpy(copy, start, element_count * sizeof(TYPE)); \
        bool result = cbe_encode_add_array_ ## NAME(g_encode_context, (TYPE*)start, ((TYPE*)start) + element_count); \
        free(copy); \
        return result; \
    }
    switch(data_type)
    {
        case CBE_TYPE_BOOLEAN:
            break;
        case CBE_TYPE_INT_8:
            return cbe_encode_add_array_int_8(g_encode_context, (int8_t*)start, ((int8_t*)start) + element_count); \
        case CBE_TYPE_INT_16:
            HANDLE_ARRAY(int_16, int16_t);
        case CBE_TYPE_INT_32:
            HANDLE_ARRAY(int_32, int32_t);
        case CBE_TYPE_INT_64:
            HANDLE_ARRAY(int_64, int64_t);
        case CBE_TYPE_INT_128:
            HANDLE_ARRAY(int_128, __int128);
        case CBE_TYPE_FLOAT_32:
            HANDLE_ARRAY(float_32, float);
        case CBE_TYPE_FLOAT_64:
            HANDLE_ARRAY(float_64, double);
        case CBE_TYPE_FLOAT_128:
            HANDLE_ARRAY(float_128, __float128);
        case CBE_TYPE_DECIMAL_32:
            HANDLE_ARRAY(decimal_32, _Decimal32);
        case CBE_TYPE_DECIMAL_64:
            HANDLE_ARRAY(decimal_64, _Decimal64);
        case CBE_TYPE_DECIMAL_128:
            HANDLE_ARRAY(decimal_128, _Decimal128);
        case CBE_TYPE_TIME:
            HANDLE_ARRAY(time, int64_t);
    }
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
    callbacks.on_array = on_array;

    g_encode_context = dst_buffer;
    cbe_decode_context* context = cbe_decode_begin(&callbacks, NULL);
    bool is_completed = cbe_decode_feed(context, src, src + src_length);
    cbe_decode_end(context);
    return is_completed;
}
