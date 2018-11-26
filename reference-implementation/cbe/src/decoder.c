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
} cbe_real_decode_process;

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

// static void report_error(cbe_decode_process* process, void* user_context, char* fmt, ...)
// {
//     va_list args;
//     va_start(args, fmt);
//     char buff[200];
//     vsprintf(buff, fmt, args);
//     process->callbacks->on_error(process, buff);
//     va_end(args);
// }

cbe_decode_process* cbe_decode_begin(cbe_decode_callbacks* callbacks, void* user_context)
{
    cbe_real_decode_process* process = malloc(sizeof(*process));
    process->callbacks = callbacks;
    process->user_context = user_context;
    return (cbe_decode_process*)process;
}

void* cbe_decode_get_user_context(cbe_decode_process* decode_process)
{
    cbe_real_decode_process* process = (cbe_real_decode_process*)decode_process;
    return process->user_context;
}

bool cbe_decode_feed(cbe_decode_process* decode_process, const uint8_t* const data_start, const int64_t byte_count)
{
    cbe_real_decode_process* process = (cbe_real_decode_process*)decode_process;
    process->current_buffer = new_buffer(data_start, data_start + byte_count);
    ro_buffer* buffer = &process->current_buffer;
    process->current_object = buffer->pos;
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
                // Ignore and restart loop because padding doesn't count as document content.
                // Otherwise the document depth test would exit the decode loop.
                continue;
            case TYPE_EMPTY:
                if(!process->callbacks->on_empty(decode_process)) return false;
                break;
            case TYPE_FALSE:
                if(!process->callbacks->on_boolean(decode_process, false)) return false;
                break;
            case TYPE_TRUE:
                if(!process->callbacks->on_boolean(decode_process, true)) return false;
                break;
            case TYPE_END_CONTAINER:
                if(!process->callbacks->on_end_container(decode_process)) return false;
                document_depth--;
                break;
            case TYPE_LIST:
                if(!process->callbacks->on_list_start(decode_process)) return false;
                document_depth++;
                break;
            case TYPE_MAP:
                if(!process->callbacks->on_map_start(decode_process)) return false;
                document_depth++;
                break;

            case TYPE_STRING_0: case TYPE_STRING_1: case TYPE_STRING_2: case TYPE_STRING_3:
            case TYPE_STRING_4: case TYPE_STRING_5: case TYPE_STRING_6: case TYPE_STRING_7:
            case TYPE_STRING_8: case TYPE_STRING_9: case TYPE_STRING_10: case TYPE_STRING_11:
            case TYPE_STRING_12: case TYPE_STRING_13: case TYPE_STRING_14: case TYPE_STRING_15:
            {
                uint64_t string_byte_count = type - TYPE_STRING_0;
                REQUEST_BYTES("string", string_byte_count)
                if(!process->callbacks->on_string(decode_process, (char*)buffer->pos, string_byte_count)) return false;
                buffer->pos += string_byte_count;
                break;
            }

            #define HANDLE_CASE_SCALAR(TYPE, NAME_FRAGMENT) \
            { \
                REQUEST_BYTES(#NAME_FRAGMENT, sizeof(TYPE)) \
                if(!process->callbacks->on_ ## NAME_FRAGMENT(decode_process, read_ ## NAME_FRAGMENT(buffer))) return false; \
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
            case TYPE_ARRAY_STRING:
            {
                REQUEST_BYTES("string", get_array_length_field_width(buffer));
                uint64_t string_byte_count = get_array_length(buffer);
                REQUEST_BYTES("string", string_byte_count);
                if(!process->callbacks->on_string(decode_process, (char*)buffer->pos, string_byte_count)) return false;
                buffer->pos += string_byte_count;
                break;
            }

            #define HANDLE_CASE_ARRAY(TYPE, CBE_TYPE) \
            { \
                REQUEST_BYTES(#TYPE " array", get_array_length_field_width(buffer)) \
                uint64_t element_count = get_array_length(buffer); \
                uint64_t array_byte_count = element_count * sizeof(TYPE); \
                REQUEST_BYTES(#TYPE " array", array_byte_count) \
                if(!process->callbacks->on_array(decode_process, CBE_TYPE, (void*)buffer->pos, element_count)) return false; \
                buffer->pos += array_byte_count; \
            }

            case TYPE_ARRAY_INT_8:
                HANDLE_CASE_ARRAY(int8_t, CBE_TYPE_INT_8);
                break;
            case TYPE_ARRAY_INT_16:
                HANDLE_CASE_ARRAY(int16_t, CBE_TYPE_INT_16);
                break;
            case TYPE_ARRAY_INT_32:
                HANDLE_CASE_ARRAY(int32_t, CBE_TYPE_INT_32);
                break;
            case TYPE_ARRAY_INT_64:
                HANDLE_CASE_ARRAY(int64_t, CBE_TYPE_INT_64);
                break;
            case TYPE_ARRAY_INT_128:
                HANDLE_CASE_ARRAY(__int128, CBE_TYPE_INT_128);
                break;
            case TYPE_ARRAY_FLOAT_32:
                HANDLE_CASE_ARRAY(float, CBE_TYPE_FLOAT_32);
                break;
            case TYPE_ARRAY_FLOAT_64:
                HANDLE_CASE_ARRAY(double, CBE_TYPE_FLOAT_64);
                break;
            case TYPE_ARRAY_FLOAT_128:
                HANDLE_CASE_ARRAY(__float128, CBE_TYPE_FLOAT_128);
                break;
            case TYPE_ARRAY_DECIMAL_32:
                HANDLE_CASE_ARRAY(_Decimal32, CBE_TYPE_DECIMAL_32);
                break;
            case TYPE_ARRAY_DECIMAL_64:
                HANDLE_CASE_ARRAY(_Decimal64, CBE_TYPE_DECIMAL_64);
                break;
            case TYPE_ARRAY_DECIMAL_128:
                HANDLE_CASE_ARRAY(_Decimal128, CBE_TYPE_DECIMAL_128);
                break;
            case TYPE_ARRAY_TIME:
                HANDLE_CASE_ARRAY(int64_t, CBE_TYPE_TIME);
                break;
            case TYPE_ARRAY_BOOLEAN:
            {
                REQUEST_BYTES("bitfield", get_array_length_field_width(buffer)) \
                uint64_t length = get_array_length(buffer);
                uint64_t array_byte_count = length / 8;
                if(array_byte_count & 7)
                {
                    array_byte_count++;
                }
                REQUEST_BYTES("bitfield", array_byte_count) \
                if(!process->callbacks->on_bitfield(decode_process, buffer->pos, length)) return false;
                buffer->pos += array_byte_count;
                break;
            }
            default:
                if(!process->callbacks->on_int_8(decode_process, type)) return false;
                break;
        }
        process->current_object = buffer->pos;
        if(document_depth <= 0)
        {
            break;
        }
    }
    return true;
}

int64_t cbe_decode_get_buffer_offset(cbe_decode_process* decode_process)
{
    cbe_real_decode_process* process = (cbe_real_decode_process*)decode_process;
    return process->current_object - process->current_buffer.start;
}

void cbe_decode_end(cbe_decode_process* process)
{
    free(process);
}
