#include <memory.h>
#include "cbe/cbe.h"
#include "cbe_internal.h"


#define RETURN_FALSE_IF_NOT_ENOUGH_ROOM(BUFFER, REQUIRED_BYTES) \
    if((size_t)((BUFFER)->end - (BUFFER)->pos) < (REQUIRED_BYTES)) return false

#define FITS_IN_INT_SMALL(VALUE)  ((VALUE) >= TYPE_SMALLINT_MIN && (VALUE) <= TYPE_SMALLINT_MAX)
#define FITS_IN_INT_8(VALUE)      ((VALUE) == (int8_t)(VALUE))
#define FITS_IN_INT_16(VALUE)     ((VALUE) == (int16_t)(VALUE))
#define FITS_IN_INT_32(VALUE)     ((VALUE) == (int32_t)(VALUE))
#define FITS_IN_INT_64(VALUE)     ((VALUE) == (int64_t)(VALUE))
#define FITS_IN_FLOAT_32(VALUE)   ((VALUE) == (double)(float)(VALUE))
#define FITS_IN_FLOAT_64(VALUE)   ((VALUE) == (double)(VALUE))
#define FITS_IN_DECIMAL_64(VALUE) ((VALUE) == (_Decimal64)(VALUE))

static inline unsigned int compacted_array_length_field_width(const uint64_t length)
{
    if(length <= ARRAY_LENGTH_SMALL_MAX)
    {
        return 0;
    }
    if(length <= UINT8_MAX)
    {
        return 1;
    }
    if(length <= UINT16_MAX)
    {
        return 2;
    }
    if(length <= UINT32_MAX)
    {
        return 4;
    }
    return 8;
}

static inline unsigned int compacted_bytes_length_field_width(const uint64_t length)
{
    if(length <= BYTES_LENGTH_8_BIT_MAX)
    {
        return 1;
    }
    if(length <= UINT16_MAX)
    {
        return 3;
    }
    if(length <= UINT32_MAX)
    {
        return 5;
    }
    return 9;
}

static inline void add_primitive_uint_8(cbe_encode_buffer* const buffer, const uint8_t value)
{
    *buffer->pos++ = value;
}
static inline void add_primitive_int_8(cbe_encode_buffer* const buffer, const int8_t value)
{
    add_primitive_uint_8(buffer, (uint8_t)value);
}
#define DEFINE_PRIMITIVE_ADD_FUNCTION(DATA_TYPE, DEFINITION_TYPE) \
static inline void add_primitive_ ## DEFINITION_TYPE(cbe_encode_buffer* const buffer, const DATA_TYPE value) \
{ \
    /* Must clear memory first because the compiler may do a partial store where there are zero bytes in the source */ \
    memset(buffer->pos, 0, sizeof(value)); \
    safe_ ## DEFINITION_TYPE* safe = (safe_##DEFINITION_TYPE*)buffer->pos; \
    safe->contents = value; \
    buffer->pos += sizeof(value); \
}
DEFINE_PRIMITIVE_ADD_FUNCTION(uint16_t,      uint_16)
DEFINE_PRIMITIVE_ADD_FUNCTION(uint32_t,      uint_32)
DEFINE_PRIMITIVE_ADD_FUNCTION(uint64_t,      uint_64)
DEFINE_PRIMITIVE_ADD_FUNCTION(int16_t,        int_16)
DEFINE_PRIMITIVE_ADD_FUNCTION(int32_t,        int_32)
DEFINE_PRIMITIVE_ADD_FUNCTION(int64_t,        int_64)
DEFINE_PRIMITIVE_ADD_FUNCTION(__int128,      int_128)
DEFINE_PRIMITIVE_ADD_FUNCTION(float,        float_32)
DEFINE_PRIMITIVE_ADD_FUNCTION(double,       float_64)
DEFINE_PRIMITIVE_ADD_FUNCTION(long double, float_128)
static inline void add_primitive_bytes(cbe_encode_buffer* const buffer,
                                       const uint8_t* const bytes,
                                       const unsigned int byte_count)
{
    memcpy(buffer->pos, bytes, byte_count);
    buffer->pos += byte_count;
}

static inline void add_primitive_type(cbe_encode_buffer* const buffer, const type_field type)
{
    add_primitive_int_8(buffer, (int8_t)type);
}

static inline void add_primitive_byte_array_length(cbe_encode_buffer* const buffer, const uint64_t length)
{
    if(length <= BYTES_LENGTH_8_BIT_MAX)
    {
        add_primitive_uint_8(buffer, (uint8_t)length);
        return;
    }
    if(length <= UINT16_MAX)
    {
        add_primitive_uint_8(buffer, (uint8_t)BYTES_LENGTH_16_BIT);
        add_primitive_uint_16(buffer, length);
        return;
    }
    if(length <= UINT32_MAX)
    {
        add_primitive_uint_8(buffer, (uint8_t)BYTES_LENGTH_32_BIT);
        add_primitive_uint_32(buffer, length);
        return;
    }
    add_primitive_uint_8(buffer, (uint8_t)BYTES_LENGTH_64_BIT);
    add_primitive_uint_64(buffer, length);
}

static inline void add_primitive_array_content_type_and_length(cbe_encode_buffer* const buffer,
                                                               const array_content_type_field type,
                                                               const uint64_t length)
{
    if(length <= ARRAY_LENGTH_SMALL_MAX)
    {
        add_primitive_uint_8(buffer, (uint8_t)(length | type));
        return;
    }
    if(length <= UINT8_MAX)
    {
        add_primitive_uint_8(buffer, (uint8_t)(ARRAY_LENGTH_8_BIT | type));
        add_primitive_uint_8(buffer, length);
        return;
    }
    if(length <= UINT16_MAX)
    {
        add_primitive_uint_8(buffer, (uint8_t)(ARRAY_LENGTH_16_BIT | type));
        add_primitive_uint_16(buffer, length);
        return;
    }
    if(length <= UINT32_MAX)
    {
        add_primitive_uint_8(buffer, (uint8_t)(ARRAY_LENGTH_32_BIT | type));
        add_primitive_uint_32(buffer, length);
        return;
    }
    add_primitive_uint_8(buffer, (uint8_t)(ARRAY_LENGTH_64_BIT | type));
    add_primitive_uint_64(buffer, length);
}


static inline bool add_small(cbe_encode_buffer* const buffer, const int8_t value)
{
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, sizeof(value));
    add_primitive_type(buffer, value);
    return true;
}

#define DEFINE_SCALAR_ADD_FUNCTION(DATA_TYPE, DEFINITION_TYPE, CBE_TYPE) \
static inline bool add_ ## DEFINITION_TYPE(cbe_encode_buffer* const buffer, const DATA_TYPE value) \
{ \
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, sizeof(value) + sizeof(CBE_TYPE)); \
    add_primitive_type(buffer, CBE_TYPE); \
    add_primitive_ ## DEFINITION_TYPE(buffer, value); \
    return true; \
}
DEFINE_SCALAR_ADD_FUNCTION(int16_t,        int_16, TYPE_INT_16)
DEFINE_SCALAR_ADD_FUNCTION(int32_t,        int_32, TYPE_INT_32)
DEFINE_SCALAR_ADD_FUNCTION(int64_t,        int_64, TYPE_INT_64)
DEFINE_SCALAR_ADD_FUNCTION(__int128,      int_128, TYPE_INT_128)
DEFINE_SCALAR_ADD_FUNCTION(float,        float_32, TYPE_FLOAT_32)
DEFINE_SCALAR_ADD_FUNCTION(double,       float_64, TYPE_FLOAT_64)
DEFINE_SCALAR_ADD_FUNCTION(long double, float_128, TYPE_FLOAT_128)

static inline bool add_lowbytes(cbe_encode_buffer* const buffer,
                                const uint8_t type,
                                const unsigned int byte_count,
                                const uint64_t data)
{
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, byte_count + sizeof(type));
    add_primitive_type(buffer, type);
    add_primitive_bytes(buffer, (uint8_t*)&data, byte_count);
    return true;
}

static inline bool add_bytes_with_type(cbe_encode_buffer* const buffer,
                                       const type_field short_type,
                                       const type_field long_type,
                                       const uint8_t* const bytes,
                                       const int byte_count)
{
    if(byte_count <= 15)
    {
        const uint8_t type = short_type + byte_count;
        RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, sizeof(type) + byte_count);
        add_primitive_type(buffer, type);
    }
    else
    {
        const uint8_t type = long_type;
        RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, sizeof(type) + byte_count + compacted_bytes_length_field_width(byte_count));
        add_primitive_type(buffer, type);
        add_primitive_byte_array_length(buffer, byte_count);
    }
    add_primitive_bytes(buffer, bytes, byte_count);
    return true;
}

void cbe_init_buffer(cbe_encode_buffer* const buffer, uint8_t* const memory_start, uint8_t* const memory_end)
{
    buffer->start = memory_start;
    buffer->end = memory_end;
    buffer->pos = memory_start;
}

bool cbe_add_boolean(cbe_encode_buffer* const buffer, const bool value)
{
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, sizeof(value));
    add_primitive_type(buffer, value ? TYPE_TRUE : TYPE_FALSE);
    return true;
}

bool cbe_add_int(cbe_encode_buffer* const buffer, const int value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    return false;
}

bool cbe_add_int_8(cbe_encode_buffer* const buffer, int8_t const value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    return add_int_16(buffer, value);
}

bool cbe_add_int_16(cbe_encode_buffer* const buffer, int16_t const value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    return add_int_16(buffer, value);
}

bool cbe_add_int_32(cbe_encode_buffer* const buffer, int32_t const value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    if(FITS_IN_INT_16(value)) return add_int_16(buffer, value);
    return add_int_32(buffer, value);
}

bool cbe_add_int_64(cbe_encode_buffer* const buffer, int64_t const value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    if(FITS_IN_INT_16(value)) return add_int_16(buffer, value);
    if(FITS_IN_INT_32(value)) return add_int_32(buffer, value);
    return add_int_64(buffer, value);
}

bool cbe_add_int_128(cbe_encode_buffer* const buffer, const __int128 value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    if(FITS_IN_INT_16(value)) return add_int_16(buffer, value);
    if(FITS_IN_INT_32(value)) return add_int_32(buffer, value);
    if(FITS_IN_INT_64(value)) return add_int_64(buffer, value);
    return add_int_128(buffer, value);
}

bool cbe_add_float_32(cbe_encode_buffer* const buffer, const float value)
{
    return add_float_32(buffer, value);
}

bool cbe_add_float_64(cbe_encode_buffer* const buffer, const double value)
{
    if(FITS_IN_FLOAT_32(value)) return add_float_32(buffer, value);
    return add_float_64(buffer, value);
}

bool cbe_add_float_128(cbe_encode_buffer* const buffer, const long double value)
{
    if(FITS_IN_FLOAT_32(value)) return add_float_32(buffer, value);
    if(FITS_IN_FLOAT_64(value)) return add_float_64(buffer, value);
    return add_float_128(buffer, value);
}

bool cbe_add_date(cbe_encode_buffer* const buffer,
                  const unsigned year,
                  const unsigned month,
                  const unsigned day,
                  const unsigned hour,
                  const unsigned minute,
                  const unsigned second)
{
    return add_lowbytes(buffer, TYPE_DATE, 40/8, 
                        year      * DATE_MULTIPLIER_YEAR +
                        (month-1) * DATE_MULTIPLIER_MONTH +
                        (day-1)   * DATE_MULTIPLIER_DAY +
                        hour      * DATE_MULTIPLIER_HOUR +
                        minute    * DATE_MULTIPLIER_MINUTE +
                        second    * DATE_MULTIPLIER_SECOND);
}

bool cbe_add_timestamp_ms(cbe_encode_buffer* const buffer,
                          const unsigned year,
                          const unsigned month,
                          const unsigned day,
                          const unsigned hour,
                          const unsigned minute,
                          const unsigned second,
                          const unsigned millisecond)
{
    return add_lowbytes(buffer, TYPE_TIMESTAMP_MILLI, 48/8, 
                        year        * TS_MS_MULTIPLIER_YEAR +
                        (month-1)   * TS_MS_MULTIPLIER_MONTH +
                        (day-1)     * TS_MS_MULTIPLIER_DAY +
                        hour        * TS_MS_MULTIPLIER_HOUR +
                        minute      * TS_MS_MULTIPLIER_MINUTE +
                        second      * TS_MS_MULTIPLIER_SECOND +
                        millisecond * TS_MS_MULTIPLIER_MILLISECOND);
}

bool cbe_add_timestamp_us(cbe_encode_buffer* buffer,
                          const unsigned year,
                          const unsigned month,
                          const unsigned day,
                          const unsigned hour,
                          const unsigned minute,
                          const unsigned second,
                          const unsigned microsecond)
{
    return add_lowbytes(buffer, TYPE_TIMESTAMP_MICRO, 64/8, 
                        year        * TS_US_MULTIPLIER_YEAR +
                        (month-1)   * TS_US_MULTIPLIER_MONTH +
                        (day-1)     * TS_US_MULTIPLIER_DAY +
                        hour        * TS_US_MULTIPLIER_HOUR +
                        minute      * TS_US_MULTIPLIER_MINUTE +
                        second      * TS_US_MULTIPLIER_SECOND +
                        microsecond * TS_US_MULTIPLIER_MICROSECOND);
}

bool cbe_add_string(cbe_encode_buffer* const buffer, const char* const value)
{
    return add_bytes_with_type(buffer, TYPE_STRING_0, TYPE_STRING, (const uint8_t*)value, strlen(value));
}

bool cbe_add_bytes(cbe_encode_buffer* const buffer, const uint8_t* const value, const int byte_count)
{
    return add_bytes_with_type(buffer, TYPE_BYTES_0, TYPE_BYTES, value, byte_count);
}

bool cbe_start_list(cbe_encode_buffer* const buffer)
{
    const uint8_t type = TYPE_LIST;
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, sizeof(type) * 2);
    add_primitive_type(buffer, type);
    return true;
}

bool cbe_start_map(cbe_encode_buffer* const buffer)
{
    const uint8_t type = TYPE_MAP;
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, sizeof(type) * 2);
    add_primitive_type(buffer, type);
    return true;
}

bool cbe_end_container(cbe_encode_buffer* const buffer)
{
    const uint8_t type = TYPE_END_CONTAINER;
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, sizeof(type));
    add_primitive_type(buffer, type);
    return true;
}

static bool add_array(cbe_encode_buffer* const buffer,
                      const type_field content_type,
                      const uint8_t* const values,
                      const int entity_count,
                      const int entity_size)
{
    const uint8_t type = TYPE_ARRAY;
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer,
                                    sizeof(type) +
                                    compacted_array_length_field_width(entity_count) +
                                    entity_count * entity_size);
    add_primitive_type(buffer, type);
    add_primitive_array_content_type_and_length(buffer, content_type, entity_count);
    add_primitive_bytes(buffer, values, entity_count * entity_size);
    return true;
}


bool cbe_add_array_int_16(cbe_encode_buffer* const buffer, const int16_t* const values, const int entity_count)
{
    return add_array(buffer, TYPE_INT_16, (const uint8_t* const)values, entity_count, sizeof(*values));
}

bool cbe_add_array_int_32(cbe_encode_buffer* const buffer, const int32_t* const values, const int entity_count)
{
    return add_array(buffer, TYPE_INT_32, (const uint8_t* const)values, entity_count, sizeof(*values));
}

bool cbe_add_array_int_64(cbe_encode_buffer* const buffer, const int64_t* const values, const int entity_count)
{
    return add_array(buffer, TYPE_INT_64, (const uint8_t* const)values, entity_count, sizeof(*values));
}

bool cbe_add_array_int_128(cbe_encode_buffer* const buffer, const __int128* const values, const int entity_count)
{
    return add_array(buffer, TYPE_INT_128, (const uint8_t* const)values, entity_count, sizeof(*values));
}

bool cbe_add_array_float_32(cbe_encode_buffer* const buffer, const float* const values, const int entity_count)
{
    return add_array(buffer, TYPE_FLOAT_32, (const uint8_t* const)values, entity_count, sizeof(*values));
}

bool cbe_add_array_float_64(cbe_encode_buffer* const buffer, const double* const values, const int entity_count)
{
    return add_array(buffer, TYPE_FLOAT_64, (const uint8_t* const)values, entity_count, sizeof(*values));
}

bool cbe_add_array_float_128(cbe_encode_buffer* const buffer, const long double* const values, const int entity_count)
{
    return add_array(buffer, TYPE_FLOAT_128, (const uint8_t* const)values, entity_count, sizeof(*values));
}

bool cbe_add_array_boolean(cbe_encode_buffer* const buffer, const bool* const values, const int entity_count)
{
    const uint8_t type = TYPE_ARRAY;
    int byte_count = entity_count / 8;
    if(entity_count & 7)
    {
        byte_count++;
    }
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer,
                                    sizeof(type) +
                                    compacted_array_length_field_width(entity_count) +
                                    byte_count);
    add_primitive_type(buffer, type);
    add_primitive_array_content_type_and_length(buffer, ARRAY_CONTENT_TYPE_BOOLEAN, entity_count);
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
