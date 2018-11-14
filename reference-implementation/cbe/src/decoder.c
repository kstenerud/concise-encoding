#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include "cbe/cbe.h"
#include "cbe_internal.h"

typedef struct
{
    const uint8_t* const start;
    const uint8_t* const end;
    const uint8_t* pos;
} ro_buffer;

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

#define DEFINE_READ_UINT_IRREGULAR_FUNCTION(WIDTH) \
static inline uint64_t read_uint_ ## WIDTH(ro_buffer* const buffer) \
{ \
    const int byte_count = WIDTH/8; \
    uint64_t value = 0; \
    for(const uint8_t* pos = buffer->pos + byte_count - 1; pos >= buffer->pos; pos--) \
    { \
        value = (value << 8) | *pos; \
    } \
    buffer->pos += byte_count; \
    return value; \
}
DEFINE_READ_UINT_IRREGULAR_FUNCTION(40)
DEFINE_READ_UINT_IRREGULAR_FUNCTION(48)

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
DEFINE_READ_FUNCTION(__float128, float_128)
DEFINE_READ_FUNCTION(_Decimal32,  decimal_32)
DEFINE_READ_FUNCTION(_Decimal64,  decimal_64)
DEFINE_READ_FUNCTION(_Decimal128, decimal_128)

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

static void report_error(cbe_decode_callbacks* callbacks, char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buff[200];
    vsprintf(buff, fmt, args);
    callbacks->on_error(buff);
    va_end(args);
}

#define REQUEST_BYTES(TYPE, BYTE_COUNT) \
if(buffer->pos + (BYTE_COUNT) > buffer->end) \
{ \
    report_error(callbacks, "Type %s requires %d bytes, but only %d bytes available", \
                            TYPE, \
                            BYTE_COUNT, \
                            (int)(buffer->end - buffer->pos)); \
    return NULL; \
}

static const cbe_number g_for_sizing;

const uint8_t* cbe_decode(cbe_decode_callbacks* callbacks, const uint8_t* const data_start, const uint8_t* const data_end)
{
    ro_buffer real_buffer = new_buffer(data_start, data_end);
    ro_buffer* buffer = &real_buffer;
    while(buffer->pos < buffer->end)
    {
        cbe_type_field type = read_uint_8(buffer);
        switch(type)
        {
            case TYPE_PADDING:
                // Ignore
                break;
            case TYPE_EMPTY:
                if(!callbacks->on_empty()) return NULL;
                break;
            case TYPE_FALSE:
                if(!callbacks->on_bool(false)) return NULL;
                break;
            case TYPE_TRUE:
                if(!callbacks->on_bool(true)) return NULL;
                break;
            case TYPE_END_CONTAINER:
                if(!callbacks->on_end_container()) return NULL;
                break;
            case TYPE_LIST:
                if(!callbacks->on_list_start()) return NULL;
                break;
            case TYPE_MAP:
                if(!callbacks->on_map_start()) return NULL;
                break;
            case TYPE_TIME:
            {
                REQUEST_BYTES("time", 64/8)
                int64_t value = read_int_64(buffer);
                cbe_time time =
                {
                    .year   = value >> TIME_BITSHIFT_YEAR,
                    .day    = (value >> TIME_BITSHIFT_DAY)    & TIME_MASK_DAY,
                    .hour   = (value >> TIME_BITSHIFT_HOUR)   & TIME_MASK_HOUR,
                    .minute = (value >> TIME_BITSHIFT_MINUTE) & TIME_MASK_MINUTE,
                    .second = (value >> TIME_BITSHIFT_SECOND) & TIME_MASK_SECOND,
                    .microsecond = value & TIME_MASK_MICROSECOND,
                };
                if(!callbacks->on_time(&time)) return NULL;
                break;
            }

            case TYPE_STRING_0: case TYPE_STRING_1: case TYPE_STRING_2: case TYPE_STRING_3:
            case TYPE_STRING_4: case TYPE_STRING_5: case TYPE_STRING_6: case TYPE_STRING_7:
            case TYPE_STRING_8: case TYPE_STRING_9: case TYPE_STRING_10: case TYPE_STRING_11:
            case TYPE_STRING_12: case TYPE_STRING_13: case TYPE_STRING_14: case TYPE_STRING_15:
            {
                uint64_t byte_count = type - TYPE_STRING_0;
                REQUEST_BYTES("string", byte_count)
                char* string_start = (char*)buffer->pos;
                char* string_end = string_start + byte_count;
                if(!callbacks->on_string(string_start, string_end)) return NULL;
                buffer->pos += byte_count;
                break;
            }

            #define HANDLE_CASE_SCALAR(TYPE_UPPERCASE, TYPE_LOWERCASE) \
            { \
                REQUEST_BYTES(#TYPE_LOWERCASE, sizeof(g_for_sizing.data.TYPE_LOWERCASE)) \
                cbe_number number = \
                { \
                    .data = {.TYPE_LOWERCASE = read_ ## TYPE_LOWERCASE(buffer)}, \
                    .type = CBE_NUMERIC_ ## TYPE_UPPERCASE \
                }; \
                if(!callbacks->on_number(number)) return NULL; \
            }
            case TYPE_INT_16:
                HANDLE_CASE_SCALAR(INT_16, int_16)
                break;
            case TYPE_INT_32:
                HANDLE_CASE_SCALAR(INT_32, int_32)
                break;
            case TYPE_INT_64:
                HANDLE_CASE_SCALAR(INT_64, int_64)
                break;
            case TYPE_INT_128:
                HANDLE_CASE_SCALAR(INT_128, int_128)
                break;
            case TYPE_FLOAT_32:
                HANDLE_CASE_SCALAR(FLOAT_32, float_32)
                break;
            case TYPE_FLOAT_64:
                HANDLE_CASE_SCALAR(FLOAT_64, float_64)
                break;
            case TYPE_FLOAT_128:
                HANDLE_CASE_SCALAR(FLOAT_128, float_128)
                break;
            case TYPE_DECIMAL_32:
                HANDLE_CASE_SCALAR(DECIMAL_32, decimal_32)
                break;
            case TYPE_DECIMAL_64:
                HANDLE_CASE_SCALAR(DECIMAL_64, decimal_64)
                break;
            case TYPE_DECIMAL_128:
                HANDLE_CASE_SCALAR(DECIMAL_128, decimal_128)
                break;

            #define HANDLE_CASE_ARRAY(TYPE, HANDLER) \
            { \
                REQUEST_BYTES(#TYPE " array", get_array_length_field_width(buffer)) \
                uint64_t length = get_array_length(buffer); \
                uint64_t byte_count = length * sizeof(TYPE); \
                REQUEST_BYTES(#TYPE " array", byte_count) \
                TYPE* array_start = (TYPE*)buffer->pos; \
                TYPE* array_end = array_start + length; \
                if(!callbacks->HANDLER(array_start, array_end)) return NULL; \
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
                if(!callbacks->on_bitfield(buffer->pos, length)) return NULL;
                buffer->pos += byte_count;
                break;
            }
            default:
            {
                cbe_number number =
                {
                    .data = {.int_8 = type},
                    .type = CBE_NUMERIC_INT_8
                };
                if(!callbacks->on_number(number)) return NULL;
                break;
            }
        }
    }
    return buffer->pos;
}
