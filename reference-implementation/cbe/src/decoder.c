// TODO: partial packet handling
// TODO: EOD signaling

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "cbe/cbe.h"
#include "cbe_internal.h"

typedef struct
{
    const uint8_t* start;
    const uint8_t* end;
    const uint8_t* pos;
} ro_buffer;

typedef struct
{
    cbe_decode_callbacks* callbacks;
    ro_buffer current_buffer;
    const uint8_t* current_object;
    void* user_context;
} cbe_real_decode_context;

ro_buffer new_buffer(const uint8_t* const memory_start, const uint8_t* const memory_end)
{
    ro_buffer buffer =
    {
        .start = memory_start,
        .pos = memory_start,
        .end = memory_end
    };
    return buffer;
}


static inline uint8_t peek_uint_8(const ro_buffer* const buffer)
{
    return *buffer->pos;
}

static inline uint8_t read_uint_8(ro_buffer* const buffer)
{
    return *buffer->pos++;
}

static inline int8_t read_int_8(ro_buffer* const buffer)
{
    return (int8_t)*buffer->pos++;
}

#define DEFINE_READ_FUNCTION(TYPE, TYPE_SUFFIX) \
static inline TYPE read_ ## TYPE_SUFFIX(ro_buffer* const buffer) \
{ \
    const safe_ ## TYPE_SUFFIX* const safe = (safe_ ## TYPE_SUFFIX*)buffer->pos; \
    buffer->pos += sizeof(*safe); \
    return safe->contents; \
}
DEFINE_READ_FUNCTION(uint16_t,    uint_16)
DEFINE_READ_FUNCTION(uint32_t,    uint_32)
DEFINE_READ_FUNCTION(uint64_t,    uint_64)
DEFINE_READ_FUNCTION(int16_t,     int_16)
DEFINE_READ_FUNCTION(int32_t,     int_32)
DEFINE_READ_FUNCTION(int64_t,     int_64)
DEFINE_READ_FUNCTION(__int128,    int_128)
DEFINE_READ_FUNCTION(float,       float_32)
DEFINE_READ_FUNCTION(double,      float_64)
DEFINE_READ_FUNCTION(__float128,  float_128)
DEFINE_READ_FUNCTION(_Decimal32,  decimal_32)
DEFINE_READ_FUNCTION(_Decimal64,  decimal_64)
DEFINE_READ_FUNCTION(_Decimal128, decimal_128)
DEFINE_READ_FUNCTION(int64_t,     time)

static inline int get_array_length_field_width(const ro_buffer* const buffer)
{
    switch(peek_uint_8(buffer) & 3)
    {
        case LENGTH_FIELD_WIDTH_6_BIT:
            return 1;
        case LENGTH_FIELD_WIDTH_14_BIT:
            return 2;
        case LENGTH_FIELD_WIDTH_30_BIT:
            return 4;
        default:
            return 8;
    }
}

static inline uint64_t get_array_length(ro_buffer* const buffer)
{
    switch(peek_uint_8(buffer) & 3)
    {
        case LENGTH_FIELD_WIDTH_6_BIT:
            return read_uint_8(buffer) >> 2;
        case LENGTH_FIELD_WIDTH_14_BIT:
            return read_uint_16(buffer) >> 2;
        case LENGTH_FIELD_WIDTH_30_BIT:
            return read_uint_32(buffer) >> 2;
        default:
            return read_uint_64(buffer) >> 2;
    }
}

// static void report_error(cbe_decode_callbacks* callbacks, void* context, char* fmt, ...)
// {
//     va_list args;
//     va_start(args, fmt);
//     char buff[200];
//     vsprintf(buff, fmt, args);
//     callbacks->on_error(context, buff);
//     va_end(args);
// }

cbe_decode_context* cbe_decode_begin(cbe_decode_callbacks* callbacks, void* user_context)
{
    cbe_real_decode_context* context = malloc(sizeof(*context));
    context->callbacks = callbacks;
    context->user_context = user_context;
    return (cbe_decode_context*)context;
}

void* cbe_decode_get_user_context(cbe_decode_context* context_ptr)
{
    cbe_real_decode_context* context = (cbe_real_decode_context*)context_ptr;
    return context->user_context;
}

bool cbe_decode_feed(cbe_decode_context* context_ptr, const uint8_t* const data_start, const uint8_t* const data_end)
{
    cbe_real_decode_context* context = (cbe_real_decode_context*)context_ptr;
    context->current_buffer = new_buffer(data_start, data_end);
    ro_buffer* buffer = &context->current_buffer;
    context->current_object = buffer->pos;
    int document_depth = 0;

    #define REQUEST_BYTES(TYPE, BYTE_COUNT) \
    if(buffer->pos + (BYTE_COUNT) > buffer->end) \
    { \
        return false; \
    }

    while(buffer->pos < buffer->end)
    {
        cbe_type_field type = read_uint_8(buffer);
        switch(type)
        {
            case TYPE_PADDING:
                // Ignore
                break;
            case TYPE_EMPTY:
                if(!context->callbacks->on_empty(context_ptr)) return false;
                break;
            case TYPE_FALSE:
                if(!context->callbacks->on_boolean(context_ptr, false)) return false;
                break;
            case TYPE_TRUE:
                if(!context->callbacks->on_boolean(context_ptr, true)) return false;
                break;
            case TYPE_END_CONTAINER:
                if(!context->callbacks->on_end_container(context_ptr)) return false;
                document_depth--;
                break;
            case TYPE_LIST:
                if(!context->callbacks->on_list_start(context_ptr)) return false;
                document_depth++;
                break;
            case TYPE_MAP:
                if(!context->callbacks->on_map_start(context_ptr)) return false;
                document_depth++;
                break;

            case TYPE_STRING_0: case TYPE_STRING_1: case TYPE_STRING_2: case TYPE_STRING_3:
            case TYPE_STRING_4: case TYPE_STRING_5: case TYPE_STRING_6: case TYPE_STRING_7:
            case TYPE_STRING_8: case TYPE_STRING_9: case TYPE_STRING_10: case TYPE_STRING_11:
            case TYPE_STRING_12: case TYPE_STRING_13: case TYPE_STRING_14: case TYPE_STRING_15:
            {
                uint64_t byte_count = type - TYPE_STRING_0;
                REQUEST_BYTES("string", byte_count)
                char* string_start = (char*)buffer->pos;
                char* string_end = string_start + byte_count;
                if(!context->callbacks->on_string(context_ptr, string_start, string_end)) return false;
                buffer->pos += byte_count;
                break;
            }

            #define HANDLE_CASE_SCALAR(TYPE, NAME_FRAGMENT) \
            { \
                REQUEST_BYTES(#NAME_FRAGMENT, sizeof(TYPE)) \
                if(!context->callbacks->on_ ## NAME_FRAGMENT(context_ptr, read_ ## NAME_FRAGMENT(buffer))) return false; \
            }
            case TYPE_INT_16:
                HANDLE_CASE_SCALAR(int16_t, int_16)
                break;
            case TYPE_INT_32:
                HANDLE_CASE_SCALAR(int32_t, int_32)
                break;
            case TYPE_INT_64:
                HANDLE_CASE_SCALAR(int64_t, int_64)
                break;
            case TYPE_INT_128:
                HANDLE_CASE_SCALAR(__int128, int_128)
                break;
            case TYPE_FLOAT_32:
                HANDLE_CASE_SCALAR(float, float_32)
                break;
            case TYPE_FLOAT_64:
                HANDLE_CASE_SCALAR(double, float_64)
                break;
            case TYPE_FLOAT_128:
                HANDLE_CASE_SCALAR(__float128, float_128)
                break;
            case TYPE_DECIMAL_32:
                HANDLE_CASE_SCALAR(_Decimal32, decimal_32)
                break;
            case TYPE_DECIMAL_64:
                HANDLE_CASE_SCALAR(_Decimal64, decimal_64)
                break;
            case TYPE_DECIMAL_128:
                HANDLE_CASE_SCALAR(_Decimal128, decimal_128)
                break;
            case TYPE_TIME:
                HANDLE_CASE_SCALAR(int64_t, time)
                break;

            #define HANDLE_CASE_ARRAY(TYPE, HANDLER) \
            { \
                REQUEST_BYTES(#TYPE " array", get_array_length_field_width(buffer)) \
                uint64_t length = get_array_length(buffer); \
                uint64_t byte_count = length * sizeof(TYPE); \
                REQUEST_BYTES(#TYPE " array", byte_count) \
                TYPE* array_start = (TYPE*)buffer->pos; \
                TYPE* array_end = array_start + length; \
                if(!context->callbacks->HANDLER(context_ptr, array_start, array_end)) return false; \
                buffer->pos += byte_count; \
            }
            case TYPE_ARRAY_STRING:
                HANDLE_CASE_ARRAY(char, on_string);
                break;
            case TYPE_ARRAY_INT_8:
                HANDLE_CASE_ARRAY(int8_t, on_array_int_8);
                break;
            case TYPE_ARRAY_INT_16:
                HANDLE_CASE_ARRAY(int16_t, on_array_int_16);
                break;
            case TYPE_ARRAY_INT_32:
                HANDLE_CASE_ARRAY(int32_t, on_array_int_32);
                break;
            case TYPE_ARRAY_INT_64:
                HANDLE_CASE_ARRAY(int64_t, on_array_int_64);
                break;
            case TYPE_ARRAY_INT_128:
                HANDLE_CASE_ARRAY(__int128, on_array_int_128);
                break;
            case TYPE_ARRAY_FLOAT_32:
                HANDLE_CASE_ARRAY(float, on_array_float_32);
                break;
            case TYPE_ARRAY_FLOAT_64:
                HANDLE_CASE_ARRAY(double, on_array_float_64);
                break;
            case TYPE_ARRAY_FLOAT_128:
                HANDLE_CASE_ARRAY(__float128, on_array_float_128);
                break;
            case TYPE_ARRAY_DECIMAL_32:
                HANDLE_CASE_ARRAY(_Decimal32, on_array_decimal_32);
                break;
            case TYPE_ARRAY_DECIMAL_64:
                HANDLE_CASE_ARRAY(_Decimal64, on_array_decimal_64);
                break;
            case TYPE_ARRAY_DECIMAL_128:
                HANDLE_CASE_ARRAY(_Decimal128, on_array_decimal_128);
                break;
            case TYPE_ARRAY_TIME:
                HANDLE_CASE_ARRAY(int64_t, on_array_time);
                break;
            case TYPE_ARRAY_BOOLEAN:
            {
                REQUEST_BYTES("bitfield", get_array_length_field_width(buffer)) \
                uint64_t length = get_array_length(buffer);
                uint64_t byte_count = length / 8;
                if(byte_count & 7)
                {
                    byte_count++;
                }
                REQUEST_BYTES("bitfield", byte_count) \
                if(!context->callbacks->on_bitfield(context_ptr, buffer->pos, length)) return false;
                buffer->pos += byte_count;
                break;
            }
            default:
                if(!context->callbacks->on_int_8(context_ptr, type)) return false;
                break;
        }
        context->current_object = buffer->pos;
        if(document_depth <= 0)
        {
            break;
        }
    }
    return buffer->pos;
}

int cbe_decode_get_buffer_offset(cbe_decode_context* context_ptr)
{
    cbe_real_decode_context* context = (cbe_real_decode_context*)context_ptr;
    return context->current_object - context->current_buffer.start;
}

void cbe_decode_end(cbe_decode_context* context)
{
    free(context);
}
