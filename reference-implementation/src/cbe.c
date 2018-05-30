#include <memory.h>
#include "cbe/cbe.h"
#include "cbe_internal.h"


static const uint64_t DATE_MULTIPLIER_YEAR   = 32676480UL;
static const uint64_t DATE_MULTIPLIER_MONTH  =  2723040UL;
static const uint64_t DATE_MULTIPLIER_DAY    =    87840UL;
static const uint64_t DATE_MULTIPLIER_HOUR   =     3660UL;
static const uint64_t DATE_MULTIPLIER_MINUTE =       61UL;
static const uint64_t DATE_MULTIPLIER_SECOND =        1UL;

static const uint64_t TS_MS_MULTIPLIER_YEAR   = 32676480000UL;
static const uint64_t TS_MS_MULTIPLIER_MONTH  =  2723040000UL;
static const uint64_t TS_MS_MULTIPLIER_DAY    =    87840000UL;
static const uint64_t TS_MS_MULTIPLIER_HOUR   =     3660000UL;
static const uint64_t TS_MS_MULTIPLIER_MINUTE =       61000UL;
static const uint64_t TS_MS_MULTIPLIER_SECOND =        1000UL;
static const uint64_t TS_MS_MULTIPLIER_MILLISECOND =      1UL;

static const uint64_t TS_NS_MULTIPLIER_YEAR   = 32676480000000UL;
static const uint64_t TS_NS_MULTIPLIER_MONTH  =  2723040000000UL;
static const uint64_t TS_NS_MULTIPLIER_DAY    =    87840000000UL;
static const uint64_t TS_NS_MULTIPLIER_HOUR   =     3660000000UL;
static const uint64_t TS_NS_MULTIPLIER_MINUTE =       61000000UL;
static const uint64_t TS_NS_MULTIPLIER_SECOND =        1000000UL;
static const uint64_t TS_NS_MULTIPLIER_NANOSECOND =          1UL;

// Force the compiler to generate handler code for unaligned accesses.
#define DEFINE_SAFE_STRUCT(NAME, TYPE) typedef struct __attribute__((__packed__)) {TYPE contents;} NAME
DEFINE_SAFE_STRUCT(safe_int16, int16_t);
DEFINE_SAFE_STRUCT(safe_int32, int32_t);
DEFINE_SAFE_STRUCT(safe_int64, int64_t);
DEFINE_SAFE_STRUCT(safe_int128, __int128);
DEFINE_SAFE_STRUCT(safe_float32, float);
DEFINE_SAFE_STRUCT(safe_float64, double);
DEFINE_SAFE_STRUCT(safe_float128, long double);

#define RETURN_FALSE_IF_NOT_ENOUGH_ROOM(BUFFER, REQUIRED_BYTES) if((size_t)((BUFFER)->end - (BUFFER)->pos) < (REQUIRED_BYTES)) return false

#define FITS_IN_INT_SMALL(VALUE) ((VALUE) >= SMALLINT_MIN && (VALUE) <= SMALLINT_MAX)
#define FITS_IN_INT_8(VALUE) ((VALUE) == (int8_t)(VALUE))
#define FITS_IN_INT_16(VALUE) ((VALUE) == (int16_t)(VALUE))
#define FITS_IN_INT_32(VALUE) ((VALUE) == (int32_t)(VALUE))
#define FITS_IN_INT_64(VALUE) ((VALUE) == (int64_t)(VALUE))
#define FITS_IN_FLOAT_32(VALUE) ((VALUE) == (double)(float)(VALUE))
#define FITS_IN_FLOAT_64(VALUE) ((VALUE) == (double)(VALUE))

static inline void add_primitive_int8(cbe_buffer* const buffer, const int8_t value)
{
    *buffer->pos++ = (uint8_t)value;
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
DEFINE_PRIMITIVE_ADD_FUNCTION(int16_t,        int16)
DEFINE_PRIMITIVE_ADD_FUNCTION(int32_t,        int32)
DEFINE_PRIMITIVE_ADD_FUNCTION(int64_t,        int64)
DEFINE_PRIMITIVE_ADD_FUNCTION(__int128,      int128)
DEFINE_PRIMITIVE_ADD_FUNCTION(float,        float32)
DEFINE_PRIMITIVE_ADD_FUNCTION(double,       float64)
DEFINE_PRIMITIVE_ADD_FUNCTION(long double, float128)
static inline void add_primitive_bytes(cbe_buffer* const buffer, const uint8_t* const bytes, const unsigned int byte_count)
{
    memcpy(buffer->pos, bytes, byte_count);
    buffer->pos += byte_count;
}

static inline void add_type_field(cbe_buffer* const buffer, const type_field type)
{
    add_primitive_int8(buffer, type);
}

static inline bool add_small(cbe_buffer* const buffer, const int8_t value)
{
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, sizeof(value));
    add_type_field(buffer, value);
    return true;
}

#define DEFINE_SCALAR_ADD_FUNCTION(DATA_TYPE, DEFINITION_TYPE, CBE_TYPE) \
static inline bool add_ ## DEFINITION_TYPE(cbe_buffer* const buffer, const DATA_TYPE value) \
{ \
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, sizeof(value) + sizeof(CBE_TYPE)); \
    add_type_field(buffer, CBE_TYPE); \
    add_primitive_ ## DEFINITION_TYPE(buffer, value); \
    return true; \
}
DEFINE_SCALAR_ADD_FUNCTION(int16_t,        int16, TYPE_INT_16)
DEFINE_SCALAR_ADD_FUNCTION(int32_t,        int32, TYPE_INT_32)
DEFINE_SCALAR_ADD_FUNCTION(int64_t,        int64, TYPE_INT_64)
DEFINE_SCALAR_ADD_FUNCTION(__int128,      int128, TYPE_INT_128)
DEFINE_SCALAR_ADD_FUNCTION(float,        float32, TYPE_FLOAT_32)
DEFINE_SCALAR_ADD_FUNCTION(double,       float64, TYPE_FLOAT_64)
DEFINE_SCALAR_ADD_FUNCTION(long double, float128, TYPE_FLOAT_128)

static bool add_lowbytes(cbe_buffer* const buffer, const uint8_t type, const unsigned int byte_count, const uint64_t data)
{
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, byte_count + sizeof(type));
    add_type_field(buffer, type);
    add_primitive_bytes(buffer, (uint8_t*)&data, byte_count);
    return true;
}

unsigned int compacted_length_size(const uint64_t length)
{
    if(length <= LENGTH_8BIT_MAX)
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

void add_length_field(cbe_buffer* const buffer, const uint64_t length)
{
    if(length <= LENGTH_8BIT_MAX)
    {
        add_primitive_int8(buffer, (int8_t)length);
        return;
    }
    if(length <= UINT16_MAX)
    {
        add_primitive_int8(buffer, LENGTH_16BIT);
        add_primitive_int16(buffer, length);
        return;
    }
    if(length <= UINT32_MAX)
    {
        add_primitive_int8(buffer, LENGTH_32BIT);
        add_primitive_int32(buffer, length);
        return;
    }
    add_primitive_int8(buffer, LENGTH_64BIT);
    add_primitive_int64(buffer, length);
}

static bool add_bytes_with_type(cbe_buffer* const buffer,
                                const type_field short_type,
                                const type_field long_type,
                                const uint8_t* const bytes,
                                const int byte_count)
{
    if(byte_count <= 15)
    {
        const uint8_t type = short_type + byte_count;
        RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, sizeof(type) + byte_count);
        add_type_field(buffer, type);
    }
    else
    {
        const uint8_t type = long_type;
        RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, sizeof(type) + byte_count + compacted_length_size(byte_count));
        add_type_field(buffer, type);
        add_length_field(buffer, byte_count);
    }
    add_primitive_bytes(buffer, bytes, byte_count);
    return true;
}

void cbe_init_buffer(cbe_buffer* const buffer, uint8_t* const memory_start, uint8_t* const memory_end)
{
    buffer->start = memory_start;
    buffer->end = memory_end;
    buffer->pos = memory_start;
}

bool cbe_add_boolean(cbe_buffer* const buffer, const bool value)
{
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, sizeof(value));
    add_type_field(buffer, value ? TYPE_TRUE : TYPE_FALSE);
    return true;
}

bool cbe_add_int(cbe_buffer* const buffer, const int value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    return false;
}

bool cbe_add_int8(cbe_buffer* const buffer, int8_t const value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    return add_int16(buffer, value);
}

bool cbe_add_int16(cbe_buffer* const buffer, int16_t const value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    return add_int16(buffer, value);
}

bool cbe_add_int32(cbe_buffer* const buffer, int32_t const value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    if(FITS_IN_INT_16(value)) return add_int16(buffer, value);
    return add_int32(buffer, value);
}

bool cbe_add_int64(cbe_buffer* const buffer, int64_t const value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    if(FITS_IN_INT_16(value)) return add_int16(buffer, value);
    if(FITS_IN_INT_32(value)) return add_int32(buffer, value);
    return add_int64(buffer, value);
}

bool cbe_add_int128(cbe_buffer* const buffer, const __int128 value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    if(FITS_IN_INT_16(value)) return add_int16(buffer, value);
    if(FITS_IN_INT_32(value)) return add_int32(buffer, value);
    if(FITS_IN_INT_64(value)) return add_int64(buffer, value);
    return add_int128(buffer, value);
}

bool cbe_add_float32(cbe_buffer* const buffer, const float value)
{
    return add_float32(buffer, value);
}

bool cbe_add_float64(cbe_buffer* const buffer, const double value)
{
    if(FITS_IN_FLOAT_32(value)) return add_float32(buffer, value);
    return add_float64(buffer, value);
}

bool cbe_add_float128(cbe_buffer* const buffer, const long double value)
{
    if(FITS_IN_FLOAT_32(value)) return add_float32(buffer, value);
    if(FITS_IN_FLOAT_64(value)) return add_float64(buffer, value);
    return add_float128(buffer, value);
}

bool cbe_add_date(cbe_buffer* const buffer,
                    const unsigned year,
                    const unsigned month,
                    const unsigned day,
                    const unsigned hour,
                    const unsigned minute,
                    const unsigned second)
{
    return add_lowbytes(buffer, TYPE_DATE, 40/8, 
                        year * DATE_MULTIPLIER_YEAR +
                        (month-1) * DATE_MULTIPLIER_MONTH +
                        (day-1) * DATE_MULTIPLIER_DAY +
                        hour * DATE_MULTIPLIER_HOUR +
                        minute * DATE_MULTIPLIER_MINUTE +
                        second * DATE_MULTIPLIER_SECOND);
}

bool cbe_add_timestamp(cbe_buffer* const buffer,
                        const unsigned year,
                        const unsigned month,
                        const unsigned day,
                        const unsigned hour,
                        const unsigned minute,
                        const unsigned second,
                        const unsigned msec)
{
    return add_lowbytes(buffer, TYPE_TIMESTAMP_MS, 48/8, 
                        year * TS_MS_MULTIPLIER_YEAR +
                        (month-1) * TS_MS_MULTIPLIER_MONTH +
                        (day-1) * TS_MS_MULTIPLIER_DAY +
                        hour * TS_MS_MULTIPLIER_HOUR +
                        minute * TS_MS_MULTIPLIER_MINUTE +
                        second * TS_MS_MULTIPLIER_SECOND +
                        msec * TS_MS_MULTIPLIER_MILLISECOND);
}

bool cbe_add_timestamp_ns(cbe_buffer* buffer,
                            const unsigned year,
                            const unsigned month,
                            const unsigned day,
                            const unsigned hour,
                            const unsigned minute,
                            const unsigned second,
                            const unsigned nsec)
{
    return add_lowbytes(buffer, TYPE_TIMESTAMP_NS, 64/8, 
                        year * TS_NS_MULTIPLIER_YEAR +
                        (month-1) * TS_NS_MULTIPLIER_MONTH +
                        (day-1) * TS_NS_MULTIPLIER_DAY +
                        hour * TS_NS_MULTIPLIER_HOUR +
                        minute * TS_NS_MULTIPLIER_MINUTE +
                        second * TS_NS_MULTIPLIER_SECOND +
                        nsec * TS_NS_MULTIPLIER_NANOSECOND);
}

bool cbe_add_string(cbe_buffer* const buffer, const char* const value)
{
    return add_bytes_with_type(buffer, TYPE_STRING_0, TYPE_STRING, (const uint8_t*)value, strlen(value));
}

bool cbe_add_bytes(cbe_buffer* const buffer, const uint8_t* const value, const int byte_count)
{
    return add_bytes_with_type(buffer, TYPE_BYTES_0, TYPE_BYTES, value, byte_count);
}

// todo: arrays

bool cbe_start_list(cbe_buffer* const buffer)
{
    return buffer == NULL;
}

bool cbe_start_map(cbe_buffer* const buffer)
{
    return buffer == NULL;
}

bool cbe_end_container(cbe_buffer* const buffer)
{
    return buffer == NULL;
}
