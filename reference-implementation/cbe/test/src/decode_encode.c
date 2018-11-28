#include <stdlib.h>
#include <cbe/cbe.h>
#include <memory.h>
#include "decode_encode.h"

static bool on_empty(cbe_decode_process* decode_process)
{
    cbe_encode_process* encode_process = (cbe_encode_process*)cbe_decode_get_user_context(decode_process);
    return cbe_encode_add_empty(encode_process) == CBE_ENCODE_STATUS_OK;
}

static bool on_boolean(cbe_decode_process* decode_process, bool value)
{
    cbe_encode_process* encode_process = (cbe_encode_process*)cbe_decode_get_user_context(decode_process);
    return cbe_encode_add_boolean(encode_process, value) == CBE_ENCODE_STATUS_OK;
}

#define DEFINE_ADD_PRIMITIVE(TYPE, NAME_FRAGMENT) \
static bool on_ ## NAME_FRAGMENT(cbe_decode_process* decode_process, TYPE value) \
{ \
    cbe_encode_process* encode_process = (cbe_encode_process*)cbe_decode_get_user_context(decode_process); \
    return cbe_encode_add_ ## NAME_FRAGMENT(encode_process, value) == CBE_ENCODE_STATUS_OK; \
}

DEFINE_ADD_PRIMITIVE(int8_t,      int_8)
DEFINE_ADD_PRIMITIVE(int16_t,     int_16)
DEFINE_ADD_PRIMITIVE(int32_t,     int_32)
DEFINE_ADD_PRIMITIVE(int64_t,     int_64)
DEFINE_ADD_PRIMITIVE(__int128,    int_128)
DEFINE_ADD_PRIMITIVE(float,       float_32)
DEFINE_ADD_PRIMITIVE(double,      float_64)
DEFINE_ADD_PRIMITIVE(__float128,  float_128)
DEFINE_ADD_PRIMITIVE(_Decimal32,  decimal_32)
DEFINE_ADD_PRIMITIVE(_Decimal64,  decimal_64)
DEFINE_ADD_PRIMITIVE(_Decimal128, decimal_128)
DEFINE_ADD_PRIMITIVE(smalltime,   time)

bool on_end_container(cbe_decode_process* decode_process)
{
    cbe_encode_process* encode_process = (cbe_encode_process*)cbe_decode_get_user_context(decode_process);
    return cbe_encode_end_container(encode_process) == CBE_ENCODE_STATUS_OK;
}

bool on_begin_list(cbe_decode_process* decode_process)
{
    cbe_encode_process* encode_process = (cbe_encode_process*)cbe_decode_get_user_context(decode_process);
    return cbe_encode_begin_list(encode_process) == CBE_ENCODE_STATUS_OK;
}

bool on_begin_map(cbe_decode_process* decode_process)
{
    cbe_encode_process* encode_process = (cbe_encode_process*)cbe_decode_get_user_context(decode_process);
    return cbe_encode_begin_map(encode_process) == CBE_ENCODE_STATUS_OK;
}

bool on_bitfield(cbe_decode_process* decode_process, const uint8_t* elements, const int64_t element_count)
{
    cbe_encode_process* encode_process = (cbe_encode_process*)cbe_decode_get_user_context(decode_process);
    return cbe_encode_add_bitfield(encode_process, elements, element_count) == CBE_ENCODE_STATUS_OK;
}

bool on_string(cbe_decode_process* decode_process, const char* string_start, const int64_t byte_count)
{
    cbe_encode_process* encode_process = (cbe_encode_process*)cbe_decode_get_user_context(decode_process);
    return cbe_encode_add_substring(encode_process, string_start, byte_count) == CBE_ENCODE_STATUS_OK;
}

bool on_array(cbe_decode_process* decode_process, cbe_data_type data_type, const void* elements, int64_t element_count)
{
    cbe_encode_process* encode_process = (cbe_encode_process*)cbe_decode_get_user_context(decode_process);
    #define HANDLE_ARRAY(NAME, TYPE) \
    { \
        void* elements_copy = malloc(element_count * sizeof(TYPE)); \
        memcpy(elements_copy, elements, element_count * sizeof(TYPE)); \
        cbe_encode_status result = cbe_encode_add_array_ ## NAME(encode_process, (TYPE*)elements_copy, element_count); \
        free(elements_copy); \
        return result == CBE_ENCODE_STATUS_OK; \
    }
    switch(data_type)
    {
        case CBE_TYPE_BOOLEAN_8:
            break;
        case CBE_TYPE_INT_8:
            return cbe_encode_add_array_int_8(encode_process, (int8_t*)elements, element_count) == CBE_ENCODE_STATUS_OK; \
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
        case CBE_TYPE_TIME_64:
            HANDLE_ARRAY(time, smalltime);
    }
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
    callbacks->on_bitfield = on_bitfield;
    callbacks->on_string = on_string;
    callbacks->on_array = on_array;
    return cbe_decode_begin(callbacks, encode_process);
}

cbe_encode_status decode_encode(const uint8_t* document, int byte_count, cbe_encode_process* encode_process)
{
    cbe_decode_callbacks callbacks;
    cbe_decode_process* decode_process = new_decode_encode_process(&callbacks, encode_process);
    cbe_decode_status status = cbe_decode_feed(decode_process, document, byte_count);
    cbe_decode_end(decode_process);
    return status;
}
