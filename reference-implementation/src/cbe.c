#include <memory.h>
#include "cbe/cbe.h"
#include "cbe_internal.h"


const uint64_t DATE_MULTIPLIER_YEAR   = 32676480UL;
const uint64_t DATE_MULTIPLIER_MONTH  =  2723040UL;
const uint64_t DATE_MULTIPLIER_DAY    =    87840UL;
const uint64_t DATE_MULTIPLIER_HOUR   =     3660UL;
const uint64_t DATE_MULTIPLIER_MINUTE =       61UL;
const uint64_t DATE_MULTIPLIER_SECOND =        1UL;

const uint64_t TS_MS_MULTIPLIER_YEAR   = 32676480000UL;
const uint64_t TS_MS_MULTIPLIER_MONTH  =  2723040000UL;
const uint64_t TS_MS_MULTIPLIER_DAY    =    87840000UL;
const uint64_t TS_MS_MULTIPLIER_HOUR   =     3660000UL;
const uint64_t TS_MS_MULTIPLIER_MINUTE =       61000UL;
const uint64_t TS_MS_MULTIPLIER_SECOND =        1000UL;
const uint64_t TS_MS_MULTIPLIER_MILLISECOND =      1UL;

const uint64_t TS_NS_MULTIPLIER_YEAR   = 32676480000000UL;
const uint64_t TS_NS_MULTIPLIER_MONTH  =  2723040000000UL;
const uint64_t TS_NS_MULTIPLIER_DAY    =    87840000000UL;
const uint64_t TS_NS_MULTIPLIER_HOUR   =     3660000000UL;
const uint64_t TS_NS_MULTIPLIER_MINUTE =       61000000UL;
const uint64_t TS_NS_MULTIPLIER_SECOND =        1000000UL;
const uint64_t TS_NS_MULTIPLIER_NANOSECOND =          1UL;

#define DEFINE_SAFE_STRUCT(NAME, TYPE) typedef struct __attribute__((__packed__)) {TYPE contents;} NAME
DEFINE_SAFE_STRUCT(safe_int16, int16_t);
DEFINE_SAFE_STRUCT(safe_int32, int32_t);
DEFINE_SAFE_STRUCT(safe_int64, int64_t);
DEFINE_SAFE_STRUCT(safe_int128, __int128);
DEFINE_SAFE_STRUCT(safe_float32, float);
DEFINE_SAFE_STRUCT(safe_float64, double);
DEFINE_SAFE_STRUCT(safe_float128, long double);

#define RETURN_FALSE_IF_NOT_ENOUGH_ROOM(BUFFER, REQUIRED_BYTES) if((size_t)((BUFFER)->end - (BUFFER)->pos) < (REQUIRED_BYTES)) return false

#define FITS_IN_INT_SMALL(VALUE) ((VALUE) >= -100 && (VALUE) <= 100)
#define FITS_IN_INT_8(VALUE) ((VALUE) == (int8_t)(VALUE))
#define FITS_IN_INT_16(VALUE) ((VALUE) == (int16_t)(VALUE))
#define FITS_IN_INT_32(VALUE) ((VALUE) == (int32_t)(VALUE))
#define FITS_IN_INT_64(VALUE) ((VALUE) == (int64_t)(VALUE))
#define FITS_IN_FLOAT_32(VALUE) ((VALUE) == (double)(float)(VALUE))
#define FITS_IN_FLOAT_64(VALUE) ((VALUE) == (double)(VALUE))

static inline void add_type_field(cbe_buffer* buffer, type_field type)
{
    *buffer->pos++ = (int8_t)type;
}

static inline bool add_small(cbe_buffer* buffer, int8_t value)
{
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, sizeof(value));
    add_type_field(buffer, value);
    return true;
}

#define DEFINE_SCALAR_ADD_FUNCTION(DATA_TYPE, DEFINITION_TYPE, CBE_TYPE) \
static inline bool add_ ## DEFINITION_TYPE(cbe_buffer* buffer, DATA_TYPE value) \
{ \
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, sizeof(value) + sizeof(CBE_TYPE)); \
    add_type_field(buffer, CBE_TYPE); \
    /* Must clear memory first because the compiler may do a partial store where there are zero bytes in the source */ \
    memset(buffer->pos, 0, sizeof(value)); \
    safe_ ## DEFINITION_TYPE* safe = (safe_##DEFINITION_TYPE*)buffer->pos; \
    safe->contents = value; \
    buffer->pos += sizeof(value); \
    return true; \
}
DEFINE_SCALAR_ADD_FUNCTION(int16_t,        int16, TYPE_INT_16)
DEFINE_SCALAR_ADD_FUNCTION(int32_t,        int32, TYPE_INT_32)
DEFINE_SCALAR_ADD_FUNCTION(int64_t,        int64, TYPE_INT_64)
DEFINE_SCALAR_ADD_FUNCTION(__int128,      int128, TYPE_INT_128)
DEFINE_SCALAR_ADD_FUNCTION(float,        float32, TYPE_FLOAT_32)
DEFINE_SCALAR_ADD_FUNCTION(double,       float64, TYPE_FLOAT_64)
DEFINE_SCALAR_ADD_FUNCTION(long double, float128, TYPE_FLOAT_128)

static bool add_lowbytes(cbe_buffer* buffer, uint8_t type, unsigned int byte_count, uint64_t data)
{
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, byte_count + sizeof(type));
    add_type_field(buffer, type);
    memcpy(buffer->pos, (uint8_t*)&data, byte_count);
    buffer->pos += byte_count;
    return true;
}

void cbe_init_buffer(cbe_buffer* buffer, uint8_t* memory_start, uint8_t* memory_end)
{
    buffer->start = memory_start;
    buffer->end = memory_end;
    buffer->pos = memory_start;
}

bool cbe_add_boolean(cbe_buffer* buffer, bool value)
{
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, sizeof(value));
    add_type_field(buffer, value ? TYPE_TRUE : TYPE_FALSE);
    return true;
}

bool cbe_add_int(cbe_buffer* buffer, int value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    return false;
}

bool cbe_add_int8(cbe_buffer* buffer, int8_t value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    return add_int16(buffer, value);
}

bool cbe_add_int16(cbe_buffer* buffer, int16_t value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    return add_int16(buffer, value);
}

bool cbe_add_int32(cbe_buffer* buffer, int32_t value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    if(FITS_IN_INT_16(value)) return add_int16(buffer, value);
    return add_int32(buffer, value);
}

bool cbe_add_int64(cbe_buffer* buffer, int64_t value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    if(FITS_IN_INT_16(value)) return add_int16(buffer, value);
    if(FITS_IN_INT_32(value)) return add_int32(buffer, value);
    return add_int64(buffer, value);
}

bool cbe_add_int128(cbe_buffer* buffer, __int128 value)
{
    if(FITS_IN_INT_SMALL(value)) return add_small(buffer, value);
    if(FITS_IN_INT_16(value)) return add_int16(buffer, value);
    if(FITS_IN_INT_32(value)) return add_int32(buffer, value);
    if(FITS_IN_INT_64(value)) return add_int64(buffer, value);
    return add_int128(buffer, value);
}

bool cbe_add_float32(cbe_buffer* buffer, float value)
{
    return add_float32(buffer, value);
}

bool cbe_add_float64(cbe_buffer* buffer, double value)
{
    if(FITS_IN_FLOAT_32(value)) return add_float32(buffer, value);
    return add_float64(buffer, value);
}

bool cbe_add_float128(cbe_buffer* buffer, long double value)
{
    if(FITS_IN_FLOAT_32(value)) return add_float32(buffer, value);
    if(FITS_IN_FLOAT_64(value)) return add_float64(buffer, value);
    return add_float128(buffer, value);
}

bool cbe_add_date(cbe_buffer* buffer, unsigned year, unsigned month, unsigned day, unsigned hour, unsigned minute, unsigned second)
{
    return add_lowbytes(buffer, TYPE_DATE, 40/8, 
                        year * DATE_MULTIPLIER_YEAR +
                        (month-1) * DATE_MULTIPLIER_MONTH +
                        (day-1) * DATE_MULTIPLIER_DAY +
                        hour * DATE_MULTIPLIER_HOUR +
                        minute * DATE_MULTIPLIER_MINUTE +
                        second * DATE_MULTIPLIER_SECOND);
}

bool cbe_add_timestamp(cbe_buffer* buffer, unsigned year, unsigned month, unsigned day, unsigned hour, unsigned minute, unsigned second, unsigned msec)
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

bool cbe_add_timestamp_ns(cbe_buffer* buffer, unsigned year, unsigned month, unsigned day, unsigned hour, unsigned minute, unsigned second, unsigned nsec)
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

bool cbe_add_string(cbe_buffer* buffer, const char* const value)
{
    return buffer == NULL && value == NULL;
}

bool cbe_add_bytes(cbe_buffer* buffer, const uint8_t* const value, int length)
{
    return buffer == NULL && value == NULL && length == 0;
}

// todo: arrays

bool cbe_start_list(cbe_buffer* buffer)
{
    return buffer == NULL;
}

bool cbe_start_map(cbe_buffer* buffer)
{
    return buffer == NULL;
}

bool cbe_end_container(cbe_buffer* buffer)
{
    return buffer == NULL;
}
