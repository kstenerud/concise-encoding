#include <memory.h>
#include "cbe/cbe.h"
#include "cbe_internal.h"


#define RETURN_FALSE_IF_NOT_ENOUGH_ROOM(BUFFER, REQUIRED_BYTES) \
    if((size_t)((BUFFER)->end - (BUFFER)->pos) < (REQUIRED_BYTES)) return false
#define RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(BUFFER, REQUIRED_BYTES) \
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(BUFFER, ((REQUIRED_BYTES) + sizeof(cbe_encoded_type_field)))

#define FITS_IN_INT_SMALL(VALUE)  ((VALUE) >= TYPE_SMALLINT_MIN && (VALUE) <= TYPE_SMALLINT_MAX)
#define FITS_IN_INT_8(VALUE)      ((VALUE) == (int8_t)(VALUE))
#define FITS_IN_INT_16(VALUE)     ((VALUE) == (int16_t)(VALUE))
#define FITS_IN_INT_32(VALUE)     ((VALUE) == (int32_t)(VALUE))
#define FITS_IN_INT_64(VALUE)     ((VALUE) == (int64_t)(VALUE))
#define FITS_IN_FLOAT_32(VALUE)   ((VALUE) == (double)(float)(VALUE))
#define FITS_IN_FLOAT_64(VALUE)   ((VALUE) == (double)(VALUE))
#define FITS_IN_DECIMAL_64(VALUE) ((VALUE) == (_Decimal64)(VALUE))

static inline unsigned int get_length_field_width(const uint64_t length)
{
    if(length <= MAX_VALUE_6_BIT)  return sizeof(uint8_t);
    if(length <= MAX_VALUE_14_BIT) return sizeof(uint16_t);
    if(length <= MAX_VALUE_30_BIT) return sizeof(uint32_t);
    return sizeof(uint64_t);
}

static inline void add_primitive_uint_8(cbe_buffer* const buffer, const uint8_t value)
{
    *buffer->pos++ = value;
}
static inline void add_primitive_int_8(cbe_buffer* const buffer, const int8_t value)
{
    add_primitive_uint_8(buffer, (uint8_t)value);
}
#define DEFINE_PRIMITIVE_ADD_FUNCTION(DATA_TYPE, DEFINITION_TYPE) \
    static inline void add_primitive_ ## DEFINITION_TYPE(cbe_buffer* const buffer, const DATA_TYPE value) \
    { \
        /* Must clear memory first because the compiler may do a partial store where there are zero bytes in the source */ \
        memset(buffer->pos, 0, sizeof(value)); \
        safe_ ## DEFINITION_TYPE* safe = (safe_##DEFINITION_TYPE*)buffer->pos; \
        safe->contents = value; \
        buffer->pos += sizeof(value); \
    }
DEFINE_PRIMITIVE_ADD_FUNCTION(uint16_t,        uint_16)
DEFINE_PRIMITIVE_ADD_FUNCTION(uint32_t,        uint_32)
DEFINE_PRIMITIVE_ADD_FUNCTION(uint64_t,        uint_64)
DEFINE_PRIMITIVE_ADD_FUNCTION(int16_t,          int_16)
DEFINE_PRIMITIVE_ADD_FUNCTION(int32_t,          int_32)
DEFINE_PRIMITIVE_ADD_FUNCTION(int64_t,          int_64)
DEFINE_PRIMITIVE_ADD_FUNCTION(__int128,        int_128)
DEFINE_PRIMITIVE_ADD_FUNCTION(float,          float_32)
DEFINE_PRIMITIVE_ADD_FUNCTION(double,         float_64)
DEFINE_PRIMITIVE_ADD_FUNCTION(__float128,   float_128)
DEFINE_PRIMITIVE_ADD_FUNCTION(_Decimal64,   decimal_64)
DEFINE_PRIMITIVE_ADD_FUNCTION(_Decimal128, decimal_128)
DEFINE_PRIMITIVE_ADD_FUNCTION(int64_t,         time_64)
static inline void add_primitive_bytes(cbe_buffer* const buffer,
                                       const uint8_t* const bytes,
                                       const unsigned int byte_count)
{
    memcpy(buffer->pos, bytes, byte_count);
    buffer->pos += byte_count;
}

static inline void add_primitive_type(cbe_buffer* const buffer, const cbe_type_field type)
{
    add_primitive_int_8(buffer, (int8_t)type);
}

static inline void add_primitive_length(cbe_buffer* const buffer, const uint64_t length)
{
    if(length <= MAX_VALUE_6_BIT)
    {
        add_primitive_uint_8(buffer, (uint8_t)((length << 2) | LENGTH_FIELD_WIDTH_6_BIT));
        return;
    }
    if(length <= MAX_VALUE_14_BIT)
    {
        add_primitive_uint_16(buffer, (uint16_t)((length << 2) | LENGTH_FIELD_WIDTH_14_BIT));
        return;
    }
    if(length <= MAX_VALUE_30_BIT)
    {
        add_primitive_uint_32(buffer, (uint32_t)((length << 2) | LENGTH_FIELD_WIDTH_30_BIT));
        return;
    }
    add_primitive_uint_64(buffer, (length << 2) | LENGTH_FIELD_WIDTH_62_BIT);
}

static inline bool add_small(cbe_buffer* const buffer, const int8_t value)
{
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(buffer, 0);
    add_primitive_type(buffer, value);
    return true;
}

#define DEFINE_ADD_SCALAR_FUNCTION(DATA_TYPE, DEFINITION_TYPE, CBE_TYPE) \
    static inline bool add_ ## DEFINITION_TYPE(cbe_buffer* const buffer, const DATA_TYPE value) \
    { \
        RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(buffer, sizeof(value)); \
        add_primitive_type(buffer, CBE_TYPE); \
        add_primitive_ ## DEFINITION_TYPE(buffer, value); \
        return true; \
    }
DEFINE_ADD_SCALAR_FUNCTION(int16_t,          int_16, TYPE_INT_16)
DEFINE_ADD_SCALAR_FUNCTION(int32_t,          int_32, TYPE_INT_32)
DEFINE_ADD_SCALAR_FUNCTION(int64_t,          int_64, TYPE_INT_64)
DEFINE_ADD_SCALAR_FUNCTION(__int128,        int_128, TYPE_INT_128)
DEFINE_ADD_SCALAR_FUNCTION(float,          float_32, TYPE_FLOAT_32)
DEFINE_ADD_SCALAR_FUNCTION(double,         float_64, TYPE_FLOAT_64)
DEFINE_ADD_SCALAR_FUNCTION(__float128,    float_128, TYPE_FLOAT_128)
DEFINE_ADD_SCALAR_FUNCTION(_Decimal64,   decimal_64, TYPE_DECIMAL_64)
DEFINE_ADD_SCALAR_FUNCTION(_Decimal128, decimal_128, TYPE_DECIMAL_128)
DEFINE_ADD_SCALAR_FUNCTION(uint64_t,        time_64, TYPE_TIME_64)

static inline bool add_time_40(cbe_buffer* const buffer, const uint64_t value)
{
    unsigned int byte_count = 40/8;
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(buffer, byte_count);
    add_primitive_type(buffer, TYPE_TIME_40);
    add_primitive_bytes(buffer, (uint8_t*)&value, byte_count);
    return true;
}

static bool add_array(cbe_buffer* const buffer,
                      const cbe_type_field array_type,
                      const uint8_t* const values,
                      const int entity_count,
                      const int entity_size)
{
    const uint8_t type = array_type;
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(buffer,
                                          get_length_field_width(entity_count) +
                                          entity_count * entity_size);
    add_primitive_type(buffer, type);
    add_primitive_length(buffer, entity_count);
    add_primitive_bytes(buffer, values, entity_count * entity_size);
    return true;
}


cbe_buffer cbe_new_buffer(uint8_t* const memory_start, uint8_t* const memory_end)
{
    cbe_buffer buffer =
    {
        .start = memory_start,
        .pos = memory_start,
        .end = memory_end,
    };
    return buffer;
}

bool cbe_add_empty(cbe_buffer* const buffer)
{
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(buffer, 0);
    add_primitive_type(buffer, TYPE_EMPTY);
    return true;
}

bool cbe_add_boolean(cbe_buffer* const buffer, const bool value)
{
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(buffer, 0);
    add_primitive_type(buffer, value ? TYPE_TRUE : TYPE_FALSE);
    return true;
}

bool cbe_add_int(cbe_buffer* const buffer, const int value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    if(FITS_IN_INT_16(value)) return add_int_16(buffer, value);
    if(FITS_IN_INT_32(value)) return add_int_32(buffer, value);
    if(FITS_IN_INT_64(value)) return add_int_64(buffer, value);
    return add_int_128(buffer, value);
}

bool cbe_add_int_8(cbe_buffer* const buffer, int8_t const value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    return add_int_16(buffer, value);
}

bool cbe_add_int_16(cbe_buffer* const buffer, int16_t const value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    return add_int_16(buffer, value);
}

bool cbe_add_int_32(cbe_buffer* const buffer, int32_t const value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    if(FITS_IN_INT_16(value)) return add_int_16(buffer, value);
    return add_int_32(buffer, value);
}

bool cbe_add_int_64(cbe_buffer* const buffer, int64_t const value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    if(FITS_IN_INT_16(value)) return add_int_16(buffer, value);
    if(FITS_IN_INT_32(value)) return add_int_32(buffer, value);
    return add_int_64(buffer, value);
}

bool cbe_add_int_128(cbe_buffer* const buffer, const __int128 value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    if(FITS_IN_INT_16(value)) return add_int_16(buffer, value);
    if(FITS_IN_INT_32(value)) return add_int_32(buffer, value);
    if(FITS_IN_INT_64(value)) return add_int_64(buffer, value);
    return add_int_128(buffer, value);
}

bool cbe_add_float_32(cbe_buffer* const buffer, const float value)
{
    return add_float_32(buffer, value);
}

bool cbe_add_float_64(cbe_buffer* const buffer, const double value)
{
    if(FITS_IN_FLOAT_32(value)) return add_float_32(buffer, value);
    return add_float_64(buffer, value);
}

bool cbe_add_float_128(cbe_buffer* const buffer, const __float128 value)
{
    if(FITS_IN_FLOAT_32(value)) return add_float_32(buffer, value);
    if(FITS_IN_FLOAT_64(value)) return add_float_64(buffer, value);
    return add_float_128(buffer, value);
}

bool cbe_add_decimal_64(cbe_buffer* const buffer, const _Decimal64 value)
{
    return add_decimal_64(buffer, value);
}

bool cbe_add_decimal_128(cbe_buffer* const buffer, const _Decimal128 value)
{
    if(FITS_IN_DECIMAL_64(value)) return add_decimal_64(buffer, value);
    return add_decimal_128(buffer, value);
}

bool cbe_add_time(cbe_buffer* const buffer, const cbe_time* const time)
{
    if(time->microsecond != 0)
    {
        uint64_t value = ((uint64_t)time->year   << TIME_64_BITSHIFT_YEAR)   |
                         ((uint64_t)time->month  << TIME_64_BITSHIFT_MONTH)  |
                         ((uint64_t)time->day    << TIME_64_BITSHIFT_DAY)    |
                         ((uint64_t)time->hour   << TIME_64_BITSHIFT_HOUR)   |
                         ((uint64_t)time->minute << TIME_64_BITSHIFT_MINUTE) |
                         ((uint64_t)time->second << TIME_64_BITSHIFT_SECOND) |
                         time->microsecond;
        return add_time_64(buffer, value);
    }

    uint64_t value = ((uint64_t)time->year   << TIME_40_BITSHIFT_YEAR)   |
                     ((uint64_t)time->month  << TIME_40_BITSHIFT_MONTH)  |
                     ((uint64_t)time->day    << TIME_40_BITSHIFT_DAY)    |
                     ((uint64_t)time->hour   << TIME_40_BITSHIFT_HOUR)   |
                     ((uint64_t)time->minute << TIME_40_BITSHIFT_MINUTE) |
                     time->second;
    return add_time_40(buffer, value);
}

bool cbe_start_list(cbe_buffer* const buffer)
{
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(buffer, sizeof(cbe_encoded_type_field));
    add_primitive_type(buffer, TYPE_LIST);
    return true;
}

bool cbe_start_map(cbe_buffer* const buffer)
{
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(buffer, sizeof(cbe_encoded_type_field));
    add_primitive_type(buffer, TYPE_MAP);
    return true;
}

bool cbe_end_container(cbe_buffer* const buffer)
{
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(buffer, 0);
    add_primitive_type(buffer, TYPE_END_CONTAINER);
    return true;
}

bool cbe_add_string(cbe_buffer* const buffer, const char* const str, const int byte_count)
{
    const int small_length_max = TYPE_STRING_MAX - TYPE_STRING_0;
    if(byte_count > small_length_max)
    {
        return add_array(buffer, TYPE_ARRAY_STRING, (const uint8_t* const)str, byte_count, sizeof(*str));
    }

    RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(buffer, byte_count);
    const uint8_t type = TYPE_STRING_0 + byte_count;
    add_primitive_type(buffer, type);
    add_primitive_bytes(buffer, (uint8_t*)str, byte_count);
    return true;
}

bool cbe_add_array_int_8(cbe_buffer* const buffer, const int8_t* const values, const int entity_count)
{
    return add_array(buffer, TYPE_ARRAY_INT_8, (const uint8_t* const)values, entity_count, sizeof(*values));
}

bool cbe_add_array_int_16(cbe_buffer* const buffer, const int16_t* const values, const int entity_count)
{
    return add_array(buffer, TYPE_ARRAY_INT_16, (const uint8_t* const)values, entity_count, sizeof(*values));
}

bool cbe_add_array_int_32(cbe_buffer* const buffer, const int32_t* const values, const int entity_count)
{
    return add_array(buffer, TYPE_ARRAY_INT_32, (const uint8_t* const)values, entity_count, sizeof(*values));
}

bool cbe_add_array_int_64(cbe_buffer* const buffer, const int64_t* const values, const int entity_count)
{
    return add_array(buffer, TYPE_ARRAY_INT_64, (const uint8_t* const)values, entity_count, sizeof(*values));
}

bool cbe_add_array_int_128(cbe_buffer* const buffer, const __int128* const values, const int entity_count)
{
    return add_array(buffer, TYPE_ARRAY_INT_128, (const uint8_t* const)values, entity_count, sizeof(*values));
}

bool cbe_add_array_float_32(cbe_buffer* const buffer, const float* const values, const int entity_count)
{
    return add_array(buffer, TYPE_ARRAY_FLOAT_32, (const uint8_t* const)values, entity_count, sizeof(*values));
}

bool cbe_add_array_float_64(cbe_buffer* const buffer, const double* const values, const int entity_count)
{
    return add_array(buffer, TYPE_ARRAY_FLOAT_64, (const uint8_t* const)values, entity_count, sizeof(*values));
}

bool cbe_add_array_float_128(cbe_buffer* const buffer, const __float128* const values, const int entity_count)
{
    return add_array(buffer, TYPE_ARRAY_FLOAT_128, (const uint8_t* const)values, entity_count, sizeof(*values));
}

bool cbe_add_array_decimal_64(cbe_buffer* const buffer, const _Decimal64* const values, const int entity_count)
{
    return add_array(buffer, TYPE_ARRAY_DECIMAL_64, (const uint8_t* const)values, entity_count, sizeof(*values));
}

bool cbe_add_array_decimal_128(cbe_buffer* const buffer, const _Decimal128* const values, const int entity_count)
{
    return add_array(buffer, TYPE_ARRAY_DECIMAL_128, (const uint8_t* const)values, entity_count, sizeof(*values));
}

bool cbe_add_bitfield(cbe_buffer* const buffer, const uint8_t* const packed_values, const int entity_count)
{
    const uint8_t type = TYPE_ARRAY_BOOLEAN;
    int byte_count = entity_count / 8;
    if(entity_count & 7)
    {
        byte_count++;
    }
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(buffer,
                                          get_length_field_width(entity_count) +
                                          byte_count);
    add_primitive_type(buffer, type);
    add_primitive_length(buffer, entity_count);
    add_primitive_bytes(buffer, packed_values, byte_count);
    return true;
}

bool cbe_add_array_boolean(cbe_buffer* const buffer, const bool* const values, const int entity_count)
{
    const uint8_t type = TYPE_ARRAY_BOOLEAN;
    int byte_count = entity_count / 8;
    if(entity_count & 7)
    {
        byte_count++;
    }
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(buffer,
                                          get_length_field_width(entity_count) +
                                          byte_count);
    add_primitive_type(buffer, type);
    add_primitive_length(buffer, entity_count);
    for(int i = 0; i < entity_count;)
    {
        uint8_t bit_pos = 1;
        uint8_t next_byte = 0;
        for(int j = 0; j < 8 && i < entity_count; j++)
        {
            if(values[i])
            {
                next_byte |= bit_pos;
            }
            bit_pos <<= 1;
            i++;
        }
        *buffer->pos++ = next_byte;
    }
    return true;
}
