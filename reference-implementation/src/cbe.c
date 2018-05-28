#include "cbe/cbe.h"
#include "cbe_internal.h"


#define DEFINE_SAFE_STRUCT(NAME, TYPE) typedef struct __attribute__((__packed__)) {TYPE contents;} NAME

#define RETURN_FALSE_IF_NOT_ENOUGH_ROOM(BUFFER, REQUIRED_BYTES) if((BUFFER)->end - (BUFFER)->pos < (REQUIRED_BYTES)) return false
#define ADD_TYPE(BUFFER, VALUE) *(BUFFER)->pos++ = (int8_t)(VALUE)

#define FITS_IN_INT_SMALL(VALUE) ((((unsigned)(VALUE)) + 100) <= 200)
#define FITS_IN_INT_8(VALUE) ((VALUE) == (int8_t)(VALUE))
#define FITS_IN_INT_16(VALUE) ((VALUE) == (int16_t)(VALUE))
#define FITS_IN_INT_32(VALUE) ((VALUE) == (int32_t)(VALUE))
#define FITS_IN_INT_64(VALUE) ((VALUE) == (int64_t)(VALUE))

DEFINE_SAFE_STRUCT(safe_int16, int16_t);
DEFINE_SAFE_STRUCT(safe_int32, int32_t);


static inline bool add_small(cbe_buffer* buffer, int8_t value)
{
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, sizeof(value));
    ADD_TYPE(buffer, value);
    return true;
}

static inline bool add_int16(cbe_buffer* buffer, int16_t value)
{
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, sizeof(value));
    ADD_TYPE(buffer, TYPE_INT_16);
    safe_int16* safe = (safe_int16*)buffer->pos;
    safe->contents = value;
    buffer->pos += sizeof(value);
    return true;
}

static inline bool add_int32(cbe_buffer* buffer, int32_t value)
{
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, sizeof(value));
    ADD_TYPE(buffer, TYPE_INT_32);
    safe_int32* safe = (safe_int32*)buffer->pos;
    safe->contents = value;
    buffer->pos += sizeof(value);
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
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(buffer, 1);
    ADD_TYPE(buffer, value ? TYPE_TRUE : TYPE_FALSE);
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
    return false;
}

bool cbe_add_int128(cbe_buffer* buffer, int128 value)
{
    return false;
}

bool cbe_add_float32(cbe_buffer* buffer, float value)
{
    return false;
}

bool cbe_add_float64(cbe_buffer* buffer, double value)
{
    return false;
}

bool cbe_add_float128(cbe_buffer* buffer, long double value)
{
    return false;
}

bool cbe_add_date(cbe_buffer* buffer, int year, int month, int day, int hour, int minute, int second)
{
    return false;
}

bool cbe_add_timestamp(cbe_buffer* buffer, int year, int month, int day, int hour, int minute, int second, int msec)
{
    return false;
}

bool cbe_add_timestamp_ns(cbe_buffer* buffer, int year, int month, int day, int hour, int minute, int second, int nsec)
{
    return false;
}

bool cbe_add_string(cbe_buffer* buffer, const char* const value)
{
    return false;
}

bool cbe_add_bytes(cbe_buffer* buffer, const uint8_t* const value, int length)
{
    return false;
}

// todo: arrays

bool cbe_start_list(cbe_buffer* buffer)
{
    return false;
}

bool cbe_start_map(cbe_buffer* buffer)
{
    return false;
}

bool cbe_end_container(cbe_buffer* buffer)
{
    return false;
}
