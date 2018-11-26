#include <memory.h>
#include <stdlib.h>
#include "cbe/cbe.h"
#include "cbe_internal.h"


typedef struct
{
    const uint8_t* start;
    const uint8_t* end;
    uint8_t* pos;
} cbe_real_encode_process;

#define RETURN_FALSE_IF_NOT_ENOUGH_ROOM(PROCESS, REQUIRED_BYTES) \
    if((size_t)((PROCESS)->end - (PROCESS)->pos) < (REQUIRED_BYTES)) return false
#define RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(PROCESS, REQUIRED_BYTES) \
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM(PROCESS, ((REQUIRED_BYTES) + sizeof(cbe_encoded_type_field)))

#define FITS_IN_INT_SMALL(VALUE)  ((VALUE) >= TYPE_SMALLINT_MIN && (VALUE) <= TYPE_SMALLINT_MAX)
#define FITS_IN_INT_8(VALUE)      ((VALUE) == (int8_t)(VALUE))
#define FITS_IN_INT_16(VALUE)     ((VALUE) == (int16_t)(VALUE))
#define FITS_IN_INT_32(VALUE)     ((VALUE) == (int32_t)(VALUE))
#define FITS_IN_INT_64(VALUE)     ((VALUE) == (int64_t)(VALUE))
#define FITS_IN_FLOAT_32(VALUE)   ((VALUE) == (double)(float)(VALUE))
#define FITS_IN_FLOAT_64(VALUE)   ((VALUE) == (double)(VALUE))
#define FITS_IN_DECIMAL_32(VALUE) ((VALUE) == (_Decimal32)(VALUE))
#define FITS_IN_DECIMAL_64(VALUE) ((VALUE) == (_Decimal64)(VALUE))

static inline unsigned int get_length_field_width(const uint64_t length)
{
    if(length <= MAX_VALUE_6_BIT)  return sizeof(uint8_t);
    if(length <= MAX_VALUE_14_BIT) return sizeof(uint16_t);
    if(length <= MAX_VALUE_30_BIT) return sizeof(uint32_t);
    return sizeof(uint64_t);
}

static inline void add_primitive_uint_8(cbe_real_encode_process* const process, const uint8_t value)
{
    *process->pos++ = value;
}
static inline void add_primitive_int_8(cbe_real_encode_process* const process, const int8_t value)
{
    add_primitive_uint_8(process, (uint8_t)value);
}
#define DEFINE_PRIMITIVE_ADD_FUNCTION(DATA_TYPE, DEFINITION_TYPE) \
    static inline void add_primitive_ ## DEFINITION_TYPE(cbe_real_encode_process* const process, const DATA_TYPE value) \
    { \
        /* Must clear memory first because the compiler may do a partial store where there are zero bytes in the source */ \
        memset(process->pos, 0, sizeof(value)); \
        safe_ ## DEFINITION_TYPE* safe = (safe_##DEFINITION_TYPE*)process->pos; \
        safe->contents = value; \
        process->pos += sizeof(value); \
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
DEFINE_PRIMITIVE_ADD_FUNCTION(__float128,    float_128)
DEFINE_PRIMITIVE_ADD_FUNCTION(_Decimal32,   decimal_32)
DEFINE_PRIMITIVE_ADD_FUNCTION(_Decimal64,   decimal_64)
DEFINE_PRIMITIVE_ADD_FUNCTION(_Decimal128, decimal_128)
DEFINE_PRIMITIVE_ADD_FUNCTION(int64_t,            time)
static inline void add_primitive_bytes(cbe_real_encode_process* const process,
                                       const uint8_t* const bytes,
                                       const uint64_t byte_count)
{
    memcpy(process->pos, bytes, byte_count);
    process->pos += byte_count;
}

static inline void add_primitive_type(cbe_real_encode_process* const process, const cbe_type_field type)
{
    add_primitive_int_8(process, (int8_t)type);
}

static inline void add_primitive_length(cbe_real_encode_process* const process, const uint64_t length)
{
    if(length <= MAX_VALUE_6_BIT)
    {
        add_primitive_uint_8(process, (uint8_t)((length << 2) | LENGTH_FIELD_WIDTH_6_BIT));
        return;
    }
    if(length <= MAX_VALUE_14_BIT)
    {
        add_primitive_uint_16(process, (uint16_t)((length << 2) | LENGTH_FIELD_WIDTH_14_BIT));
        return;
    }
    if(length <= MAX_VALUE_30_BIT)
    {
        add_primitive_uint_32(process, (uint32_t)((length << 2) | LENGTH_FIELD_WIDTH_30_BIT));
        return;
    }
    add_primitive_uint_64(process, (length << 2) | LENGTH_FIELD_WIDTH_62_BIT);
}

static inline bool add_small(cbe_real_encode_process* const process, const int8_t value)
{
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(process, 0);
    add_primitive_type(process, value);
    return true;
}

#define DEFINE_ADD_SCALAR_FUNCTION(DATA_TYPE, DEFINITION_TYPE, CBE_TYPE) \
    static inline bool add_ ## DEFINITION_TYPE(cbe_real_encode_process* const process, const DATA_TYPE value) \
    { \
        RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(process, sizeof(value)); \
        add_primitive_type(process, CBE_TYPE); \
        add_primitive_ ## DEFINITION_TYPE(process, value); \
        return true; \
    }
DEFINE_ADD_SCALAR_FUNCTION(int16_t,          int_16, TYPE_INT_16)
DEFINE_ADD_SCALAR_FUNCTION(int32_t,          int_32, TYPE_INT_32)
DEFINE_ADD_SCALAR_FUNCTION(int64_t,          int_64, TYPE_INT_64)
DEFINE_ADD_SCALAR_FUNCTION(__int128,        int_128, TYPE_INT_128)
DEFINE_ADD_SCALAR_FUNCTION(float,          float_32, TYPE_FLOAT_32)
DEFINE_ADD_SCALAR_FUNCTION(double,         float_64, TYPE_FLOAT_64)
DEFINE_ADD_SCALAR_FUNCTION(__float128,    float_128, TYPE_FLOAT_128)
DEFINE_ADD_SCALAR_FUNCTION(_Decimal32,   decimal_32, TYPE_DECIMAL_32)
DEFINE_ADD_SCALAR_FUNCTION(_Decimal64,   decimal_64, TYPE_DECIMAL_64)
DEFINE_ADD_SCALAR_FUNCTION(_Decimal128, decimal_128, TYPE_DECIMAL_128)
DEFINE_ADD_SCALAR_FUNCTION(uint64_t,           time, TYPE_TIME)

static bool add_array(cbe_real_encode_process* const process,
                      const cbe_type_field array_type,
                      const uint8_t* const values,
                      const int64_t element_count,
                      const int element_size)
{
    const uint8_t type = array_type;
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(process,
                                          get_length_field_width(element_count) +
                                          element_count * element_size);
    add_primitive_type(process, type);
    add_primitive_length(process, element_count);
    add_primitive_bytes(process, values, element_count * element_size);
    return true;
}


cbe_encode_process* cbe_encode_begin(uint8_t* const document_buffer, int64_t byte_count)
{
    cbe_real_encode_process* process = malloc(sizeof(*process));
    cbe_encode_set_buffer((cbe_encode_process*)process, document_buffer, byte_count);
    return (cbe_encode_process*)process;
}

void cbe_encode_set_buffer(cbe_encode_process* encode_process, uint8_t* const document_buffer, int64_t byte_count)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    process->start = document_buffer;
    process->pos = document_buffer;
    process->end = document_buffer + byte_count;
}

int64_t cbe_encode_get_buffer_offset(cbe_encode_process* encode_process)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    return process->pos - process->start;
}

void cbe_encode_end(cbe_encode_process* process)
{
    free(process);
}

bool cbe_encode_add_empty(cbe_encode_process* const encode_process)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(process, 0);
    add_primitive_type(process, TYPE_EMPTY);
    return true;
}

bool cbe_encode_add_boolean(cbe_encode_process* const encode_process, const bool value)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(process, 0);
    add_primitive_type(process, value ? TYPE_TRUE : TYPE_FALSE);
    return true;
}

bool cbe_encode_add_int(cbe_encode_process* const encode_process, const int value)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    if(FITS_IN_INT_SMALL(value)) return add_small(process, value);
    if(FITS_IN_INT_16(value)) return add_int_16(process, value);
    if(FITS_IN_INT_32(value)) return add_int_32(process, value);
    if(FITS_IN_INT_64(value)) return add_int_64(process, value);
    return add_int_128(process, value);
}

bool cbe_encode_add_int_8(cbe_encode_process* const encode_process, int8_t const value)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    if(FITS_IN_INT_SMALL(value)) return add_small(process, value);
    return add_int_16(process, value);
}

bool cbe_encode_add_int_16(cbe_encode_process* const encode_process, int16_t const value)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    if(FITS_IN_INT_SMALL(value)) return add_small(process, value);
    return add_int_16(process, value);
}

bool cbe_encode_add_int_32(cbe_encode_process* const encode_process, int32_t const value)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    if(FITS_IN_INT_SMALL(value)) return add_small(process, value);
    if(FITS_IN_INT_16(value)) return add_int_16(process, value);
    return add_int_32(process, value);
}

bool cbe_encode_add_int_64(cbe_encode_process* const encode_process, int64_t const value)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    if(FITS_IN_INT_SMALL(value)) return add_small(process, value);
    if(FITS_IN_INT_16(value)) return add_int_16(process, value);
    if(FITS_IN_INT_32(value)) return add_int_32(process, value);
    return add_int_64(process, value);
}

bool cbe_encode_add_int_128(cbe_encode_process* const encode_process, const __int128 value)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    if(FITS_IN_INT_SMALL(value)) return add_small(process, value);
    if(FITS_IN_INT_16(value)) return add_int_16(process, value);
    if(FITS_IN_INT_32(value)) return add_int_32(process, value);
    if(FITS_IN_INT_64(value)) return add_int_64(process, value);
    return add_int_128(process, value);
}

bool cbe_encode_add_float_32(cbe_encode_process* const encode_process, const float value)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    return add_float_32(process, value);
}

bool cbe_encode_add_float_64(cbe_encode_process* const encode_process, const double value)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    if(FITS_IN_FLOAT_32(value)) return add_float_32(process, value);
    return add_float_64(process, value);
}

bool cbe_encode_add_float_128(cbe_encode_process* const encode_process, const __float128 value)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    if(FITS_IN_FLOAT_32(value)) return add_float_32(process, value);
    if(FITS_IN_FLOAT_64(value)) return add_float_64(process, value);
    return add_float_128(process, value);
}

bool cbe_encode_add_decimal_32(cbe_encode_process* const encode_process, const _Decimal32 value)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    return add_decimal_32(process, value);
}

bool cbe_encode_add_decimal_64(cbe_encode_process* const encode_process, const _Decimal64 value)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    if(FITS_IN_DECIMAL_32(value)) return add_decimal_32(process, value);
    return add_decimal_64(process, value);
}

bool cbe_encode_add_decimal_128(cbe_encode_process* const encode_process, const _Decimal128 value)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    if(FITS_IN_DECIMAL_32(value)) return add_decimal_32(process, value);
    if(FITS_IN_DECIMAL_64(value)) return add_decimal_64(process, value);
    return add_decimal_128(process, value);
}

bool cbe_encode_add_time(cbe_encode_process* const encode_process, const int64_t value)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    return add_time(process, value);
}

bool cbe_encode_start_list(cbe_encode_process* const encode_process)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(process, sizeof(cbe_encoded_type_field));
    add_primitive_type(process, TYPE_LIST);
    return true;
}

bool cbe_encode_start_map(cbe_encode_process* const encode_process)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(process, sizeof(cbe_encoded_type_field));
    add_primitive_type(process, TYPE_MAP);
    return true;
}

bool cbe_end_container(cbe_encode_process* const encode_process)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(process, 0);
    add_primitive_type(process, TYPE_END_CONTAINER);
    return true;
}

bool cbe_encode_add_string(cbe_encode_process* const encode_process, const char* const str)
{
    return cbe_encode_add_substring(encode_process, str, strlen(str));
}

bool cbe_encode_add_substring(cbe_encode_process* const encode_process, const char* const start, const int64_t byte_count)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    const int64_t small_length_max = TYPE_STRING_15 - TYPE_STRING_0;
    if(byte_count > small_length_max)
    {
        return add_array(process, TYPE_ARRAY_STRING, (const uint8_t* const)start, byte_count, sizeof(*start));
    }

    RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(process, byte_count);
    const uint8_t type = TYPE_STRING_0 + byte_count;
    add_primitive_type(process, type);
    add_primitive_bytes(process, (uint8_t*)start, byte_count);
    return true;
}

bool cbe_encode_add_array_int_8(cbe_encode_process* const encode_process, const int8_t* const start, const int64_t element_count)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    return add_array(process, TYPE_ARRAY_INT_8, (const uint8_t* const)start, element_count, sizeof(*start));
}

bool cbe_encode_add_array_int_16(cbe_encode_process* const encode_process, const int16_t* const start, const int64_t element_count)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    return add_array(process, TYPE_ARRAY_INT_16, (const uint8_t* const)start, element_count, sizeof(*start));
}

bool cbe_encode_add_array_int_32(cbe_encode_process* const encode_process, const int32_t* const start, const int64_t element_count)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    return add_array(process, TYPE_ARRAY_INT_32, (const uint8_t* const)start, element_count, sizeof(*start));
}

bool cbe_encode_add_array_int_64(cbe_encode_process* const encode_process, const int64_t* const start, const int64_t element_count)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    return add_array(process, TYPE_ARRAY_INT_64, (const uint8_t* const)start, element_count, sizeof(*start));
}

bool cbe_encode_add_array_int_128(cbe_encode_process* const encode_process, const __int128* const start, const int64_t element_count)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    return add_array(process, TYPE_ARRAY_INT_128, (const uint8_t* const)start, element_count, sizeof(*start));
}

bool cbe_encode_add_array_float_32(cbe_encode_process* const encode_process, const float* const start, const int64_t element_count)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    return add_array(process, TYPE_ARRAY_FLOAT_32, (const uint8_t* const)start, element_count, sizeof(*start));
}

bool cbe_encode_add_array_float_64(cbe_encode_process* const encode_process, const double* const start, const int64_t element_count)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    return add_array(process, TYPE_ARRAY_FLOAT_64, (const uint8_t* const)start, element_count, sizeof(*start));
}

bool cbe_encode_add_array_float_128(cbe_encode_process* const encode_process, const __float128* const start, const int64_t element_count)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    return add_array(process, TYPE_ARRAY_FLOAT_128, (const uint8_t* const)start, element_count, sizeof(*start));
}

bool cbe_encode_add_array_decimal_32(cbe_encode_process* const encode_process, const _Decimal32* const start, const int64_t element_count)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    return add_array(process, TYPE_ARRAY_DECIMAL_32, (const uint8_t* const)start, element_count, sizeof(*start));
}

bool cbe_encode_add_array_decimal_64(cbe_encode_process* const encode_process, const _Decimal64* const start, const int64_t element_count)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    return add_array(process, TYPE_ARRAY_DECIMAL_64, (const uint8_t* const)start, element_count, sizeof(*start));
}

bool cbe_encode_add_array_decimal_128(cbe_encode_process* const encode_process, const _Decimal128* const start, const int64_t element_count)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    return add_array(process, TYPE_ARRAY_DECIMAL_128, (const uint8_t* const)start, element_count, sizeof(*start));
}

bool cbe_encode_add_array_time(cbe_encode_process* const encode_process, const int64_t* const start, const int64_t element_count)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    return add_array(process, TYPE_ARRAY_TIME, (const uint8_t* const)start, element_count, sizeof(*start));
}

bool cbe_encode_add_bitfield(cbe_encode_process* const encode_process, const uint8_t* const packed_values, const int64_t element_count)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    const uint8_t type = TYPE_ARRAY_BOOLEAN;
    int64_t byte_count = element_count / 8;
    if(element_count & 7)
    {
        byte_count++;
    }
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(process,
                                          get_length_field_width(element_count) +
                                          byte_count);
    add_primitive_type(process, type);
    add_primitive_length(process, element_count);
    add_primitive_bytes(process, packed_values, byte_count);
    return true;
}

bool cbe_encode_add_array_boolean(cbe_encode_process* const encode_process, const bool* const start, const int64_t element_count)
{
    cbe_real_encode_process* process = (cbe_real_encode_process*)encode_process;
    const uint8_t type = TYPE_ARRAY_BOOLEAN;
    int64_t byte_count = element_count / 8;
    if(element_count & 7)
    {
        byte_count++;
    }
    RETURN_FALSE_IF_NOT_ENOUGH_ROOM_TYPED(process,
                                          get_length_field_width(element_count) +
                                          byte_count);
    add_primitive_type(process, type);
    add_primitive_length(process, element_count);
    for(int64_t i = 0; i < element_count;)
    {
        uint8_t bit_pos = 1;
        uint8_t next_byte = 0;
        for(int64_t j = 0; j < 8 && i < element_count; j++)
        {
            if(start[i])
            {
                next_byte |= bit_pos;
            }
            bit_pos <<= 1;
            i++;
        }
        *process->pos++ = next_byte;
    }
    return true;
}
