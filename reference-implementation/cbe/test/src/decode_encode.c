#include <stdlib.h>
#include <cbe/cbe.h>
#include <memory.h>
#include "decode_encode.h"

#define DEFINE_DECODE_CALLBACK_SUITE_ON_MARKER(NAME_FRAGMENT, FUNCTION_FRAGMENT) \
static bool on_ ## NAME_FRAGMENT(cbe_decode_process* decode_process) \
{ \
    cbe_encode_process* encode_process = (cbe_encode_process*)cbe_decode_get_user_context(decode_process); \
    return cbe_encode_ ## FUNCTION_FRAGMENT(encode_process) == CBE_ENCODE_STATUS_OK; \
}

#define DEFINE_DECODE_CALLBACK_SUITE_ON_PRIMITIVE(TYPE, NAME_FRAGMENT) \
static bool on_ ## NAME_FRAGMENT(cbe_decode_process* decode_process, TYPE value) \
{ \
    cbe_encode_process* encode_process = (cbe_encode_process*)cbe_decode_get_user_context(decode_process); \
    cbe_encode_status status = cbe_encode_add_ ## NAME_FRAGMENT(encode_process, value); \
    return status == CBE_ENCODE_STATUS_OK; \
}

DEFINE_DECODE_CALLBACK_SUITE_ON_PRIMITIVE(bool,        boolean)
DEFINE_DECODE_CALLBACK_SUITE_ON_PRIMITIVE(int8_t,      int_8)
DEFINE_DECODE_CALLBACK_SUITE_ON_PRIMITIVE(int16_t,     int_16)
DEFINE_DECODE_CALLBACK_SUITE_ON_PRIMITIVE(int32_t,     int_32)
DEFINE_DECODE_CALLBACK_SUITE_ON_PRIMITIVE(int64_t,     int_64)
DEFINE_DECODE_CALLBACK_SUITE_ON_PRIMITIVE(__int128,    int_128)
DEFINE_DECODE_CALLBACK_SUITE_ON_PRIMITIVE(float,       float_32)
DEFINE_DECODE_CALLBACK_SUITE_ON_PRIMITIVE(double,      float_64)
DEFINE_DECODE_CALLBACK_SUITE_ON_PRIMITIVE(__float128,  float_128)
DEFINE_DECODE_CALLBACK_SUITE_ON_PRIMITIVE(_Decimal32,  decimal_32)
DEFINE_DECODE_CALLBACK_SUITE_ON_PRIMITIVE(_Decimal64,  decimal_64)
DEFINE_DECODE_CALLBACK_SUITE_ON_PRIMITIVE(_Decimal128, decimal_128)
DEFINE_DECODE_CALLBACK_SUITE_ON_PRIMITIVE(smalltime,   time)
DEFINE_DECODE_CALLBACK_SUITE_ON_MARKER(                empty, add_empty)
DEFINE_DECODE_CALLBACK_SUITE_ON_MARKER(                end_container, end_container)
DEFINE_DECODE_CALLBACK_SUITE_ON_MARKER(                begin_list, begin_list)
DEFINE_DECODE_CALLBACK_SUITE_ON_MARKER(                begin_map, begin_map)

static bool on_string(cbe_decode_process* decode_process, char* string_start, int64_t byte_count)
{
    cbe_encode_process* encode_process = (cbe_encode_process*)cbe_decode_get_user_context(decode_process);
    return cbe_encode_add_substring(encode_process, string_start, byte_count) == CBE_ENCODE_STATUS_OK;
}

static bool on_binary_data(cbe_decode_process* decode_process, uint8_t* start, int64_t byte_count)
{
    cbe_encode_process* encode_process = (cbe_encode_process*)cbe_decode_get_user_context(decode_process);
    return cbe_encode_add_binary_data(encode_process, start, byte_count) == CBE_ENCODE_STATUS_OK;
}

cbe_decode_process* new_decode_encode_process(cbe_decode_callbacks* callbacks, cbe_encode_process* encode_process)
{
    callbacks->on_empty = on_empty;
    callbacks->on_boolean = on_boolean;
    callbacks->on_int_8 = on_int_8;
    callbacks->on_int_16 = on_int_16;
    callbacks->on_int_32 = on_int_32;
    callbacks->on_int_64 = on_int_64;
    callbacks->on_int_128 = on_int_128;
    callbacks->on_float_32 = on_float_32;
    callbacks->on_float_64 = on_float_64;
    callbacks->on_float_128 = on_float_128;
    callbacks->on_decimal_32 = on_decimal_32;
    callbacks->on_decimal_64 = on_decimal_64;
    callbacks->on_decimal_128 = on_decimal_128;
    callbacks->on_time = on_time;
    callbacks->on_end_container = on_end_container;
    callbacks->on_begin_list = on_begin_list;
    callbacks->on_begin_map = on_begin_map;
    callbacks->on_string = on_string;
    callbacks->on_binary_data = on_binary_data;
    return cbe_decode_begin(callbacks, encode_process);
}

cbe_decode_status perform_decode_encode(const uint8_t* document, int byte_count, cbe_encode_process* encode_process)
{
    cbe_decode_callbacks callbacks;
    cbe_decode_process* decode_process = new_decode_encode_process(&callbacks, encode_process);
    cbe_decode_status status_feed = cbe_decode_feed(decode_process, document, byte_count);
    cbe_decode_status status_end = cbe_decode_end(decode_process);
    return status_feed != CBE_DECODE_STATUS_OK ? status_feed : status_end;
}
