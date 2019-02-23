#include "cbe_internal.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"


// ====
// Data
// ====

struct cbe_encode_process
{
    struct
    {
        const uint8_t* start;
        const uint8_t* end;
        uint8_t* position;
    } buffer;
    struct
    {
        bool is_inside_array;
        array_type type;
        int64_t current_offset;
        int64_t byte_count;
    } array;
    struct
    {
        int max_depth;
        int level;
        bool next_object_is_map_key;
        bool is_inside_map[];
    } container;
};
typedef struct cbe_encode_process cbe_encode_process;

typedef uint8_t cbe_encoded_type_field;

typedef enum
{
    ARRAY_LENGTH_FIELD_WIDTH_6_BIT  = 0,
    ARRAY_LENGTH_FIELD_WIDTH_14_BIT = 1,
    ARRAY_LENGTH_FIELD_WIDTH_30_BIT = 2,
    ARRAY_LENGTH_FIELD_WIDTH_62_BIT = 3,
} cbe_array_length_field_width;

typedef enum
{
    MAX_VALUE_6_BIT  = 0x3f,
    MAX_VALUE_14_BIT = 0x3fff,
    MAX_VALUE_30_BIT = 0x3fffffff,
} cbe_max_value;


// ==============
// Utility Macros
// ==============

#define likely_if(TEST_FOR_TRUTH) if(__builtin_expect(TEST_FOR_TRUTH, 1))
#define unlikely_if(TEST_FOR_TRUTH) if(__builtin_expect(TEST_FOR_TRUTH, 0))


// ==============
// Error Handlers
// ==============

#define STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(PROCESS, REQUIRED_BYTES) \
    unlikely_if(get_remaining_space_in_buffer(PROCESS) < (int64_t)(REQUIRED_BYTES)) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: Require %d bytes but only %d available.", \
            (REQUIRED_BYTES), get_remaining_space_in_buffer(PROCESS)); \
        return CBE_ENCODE_STATUS_NEED_MORE_ROOM; \
    }

#define STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM_WITH_TYPE(PROCESS, REQUIRED_BYTES) \
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(PROCESS, ((REQUIRED_BYTES) + sizeof(cbe_encoded_type_field)))

#define STOP_AND_EXIT_IF_IS_INSIDE_ARRAY(PROCESS) \
    unlikely_if((PROCESS)->array.is_inside_array) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: We're inside an array when we shouldn't be"); \
        return CBE_ENCODE_ERROR_INCOMPLETE_FIELD; \
    }

#define STOP_AND_EXIT_IF_IS_NOT_INSIDE_ARRAY(PROCESS) \
    unlikely_if(!(PROCESS)->array.is_inside_array) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: We're not inside an array when we should be"); \
        return CBE_ENCODE_ERROR_NOT_INSIDE_ARRAY_FIELD; \
    }

#define STOP_AND_EXIT_IF_ARRAY_LENGTH_EXCEEDED(PROCESS) \
{ \
    int64_t new_array_offset = (PROCESS)->array.current_offset + byte_count; \
    unlikely_if(new_array_offset > (PROCESS)->array.byte_count) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: Attempted to write %d bytes to array with only %d availale", \
            new_array_offset, (PROCESS)->array.byte_count); \
        return CBE_ENCODE_ERROR_FIELD_LENGTH_EXCEEDED; \
    } \
}

#define STOP_AND_EXIT_IF_MAX_CONTAINER_DEPTH_EXCEEDED(PROCESS) \
    unlikely_if((PROCESS)->container.level + 1 >= (PROCESS)->container.max_depth) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: Max depth %d exceeded", (PROCESS)->container.max_depth); \
        return CBE_ENCODE_ERROR_MAX_CONTAINER_DEPTH_EXCEEDED; \
    }

#define STOP_AND_EXIT_IF_IS_NOT_INSIDE_CONTAINER(PROCESS) \
    unlikely_if((PROCESS)->container.level <= 0) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: We're not inside a container"); \
        return CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS; \
    }

#define STOP_AND_EXIT_IF_IS_INSIDE_CONTAINER(PROCESS) \
    unlikely_if((PROCESS)->container.level != 0) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: There are still open containers when there shouldn't be"); \
        return CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS; \
    }

#define STOP_AND_EXIT_IF_MAP_VALUE_MISSING(PROCESS) \
    unlikely_if((PROCESS)->container.is_inside_map[(PROCESS)->container.level] && \
        !(PROCESS)->container.next_object_is_map_key) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: No map value provided for previous key"); \
        return CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY; \
    }

#define STOP_AND_EXIT_IF_IS_WRONG_MAP_KEY_TYPE(PROCESS) \
    unlikely_if((PROCESS)->container.is_inside_map[process->container.level] && \
        (PROCESS)->container.next_object_is_map_key) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: Map key has an invalid type"); \
        return CBE_ENCODE_ERROR_INCORRECT_KEY_TYPE; \
    }


// =======
// Utility
// =======

#define FITS_IN_INT_SMALL(VALUE)  ((VALUE) >= TYPE_SMALLINT_MIN && (VALUE) <= TYPE_SMALLINT_MAX)
#define FITS_IN_INT_8(VALUE)      ((VALUE) == (int8_t)(VALUE))
#define FITS_IN_INT_16(VALUE)     ((VALUE) == (int16_t)(VALUE))
#define FITS_IN_INT_32(VALUE)     ((VALUE) == (int32_t)(VALUE))
#define FITS_IN_INT_64(VALUE)     ((VALUE) == (int64_t)(VALUE))
#define FITS_IN_FLOAT_32(VALUE)   ((VALUE) == (double)(float)(VALUE))
#define FITS_IN_FLOAT_64(VALUE)   ((VALUE) == (double)(VALUE))
#define FITS_IN_DECIMAL_32(VALUE) ((VALUE) == (_Decimal32)(VALUE))
#define FITS_IN_DECIMAL_64(VALUE) ((VALUE) == (_Decimal64)(VALUE))

static inline int64_t minimum_int64(const int64_t a, const int64_t b)
{
    return a < b ? a : b;
}

static inline int64_t get_remaining_space_in_buffer(cbe_encode_process* const process)
{
    return process->buffer.end - process->buffer.position;
}

static inline void swap_map_key_value_status(cbe_encode_process* const process)
{
    process->container.next_object_is_map_key = !process->container.next_object_is_map_key;
}

static inline int get_array_length_field_width(const int64_t length)
{
    if(length <= MAX_VALUE_6_BIT)  return sizeof(int8_t);
    if(length <= MAX_VALUE_14_BIT) return sizeof(int16_t);
    if(length <= MAX_VALUE_30_BIT) return sizeof(int32_t);
    return sizeof(int64_t);
}

static inline void add_primitive_type(cbe_encode_process* const process, const cbe_type_field type)
{
    KSLOG_DEBUG("[%02x]", type);

    *process->buffer.position++ = (uint8_t)type;
}
static inline void add_primitive_uint_8(cbe_encode_process* const process, const uint8_t value)
{
    KSLOG_DEBUG("[%02x]", value);

    *process->buffer.position++ = value;
}
static inline void add_primitive_int_8(cbe_encode_process* const process, const int8_t value)
{
    KSLOG_DEBUG("[%02x] (%d)", value & 0xff, value);

    *process->buffer.position++ = (uint8_t)value;
}
#define DEFINE_PRIMITIVE_ADD_FUNCTION(DATA_TYPE, DEFINITION_TYPE) \
    static inline void add_primitive_ ## DEFINITION_TYPE(cbe_encode_process* const process, const DATA_TYPE value) \
    { \
        safe_ ## DEFINITION_TYPE* safe = (safe_##DEFINITION_TYPE*)process->buffer.position; \
        safe->contents = value; \
        KSLOG_DATA_DEBUG(process->buffer.position, sizeof(value), NULL); \
        process->buffer.position += sizeof(value); \
    }
DEFINE_PRIMITIVE_ADD_FUNCTION(uint16_t,    uint_16)
DEFINE_PRIMITIVE_ADD_FUNCTION(uint32_t,    uint_32)
DEFINE_PRIMITIVE_ADD_FUNCTION(uint64_t,    uint_64)
DEFINE_PRIMITIVE_ADD_FUNCTION(int16_t,     int_16)
DEFINE_PRIMITIVE_ADD_FUNCTION(int32_t,     int_32)
DEFINE_PRIMITIVE_ADD_FUNCTION(int64_t,     int_64)
DEFINE_PRIMITIVE_ADD_FUNCTION(__int128,    int_128)
DEFINE_PRIMITIVE_ADD_FUNCTION(float,       float_32)
DEFINE_PRIMITIVE_ADD_FUNCTION(double,      float_64)
DEFINE_PRIMITIVE_ADD_FUNCTION(__float128,  float_128)
DEFINE_PRIMITIVE_ADD_FUNCTION(_Decimal32,  decimal_32)
DEFINE_PRIMITIVE_ADD_FUNCTION(_Decimal64,  decimal_64)
DEFINE_PRIMITIVE_ADD_FUNCTION(_Decimal128, decimal_128)
DEFINE_PRIMITIVE_ADD_FUNCTION(smalltime,   time)
static inline void add_primitive_bytes(cbe_encode_process* const process,
                                       const uint8_t* const bytes,
                                       const int64_t byte_count)
{
    if(byte_count < 50)
    {
        KSLOG_DATA_DEBUG(bytes, byte_count, "%d Bytes: ", byte_count);
    }
    else
    {
        KSLOG_DATA_TRACE(bytes, byte_count, "%d Bytes: ", byte_count);
    }

    memcpy(process->buffer.position, bytes, byte_count);
    process->buffer.position += byte_count;
}

static inline void begin_array(cbe_encode_process* const process, array_type type, int64_t byte_count)
{
    KSLOG_DEBUG("(process %p, type %d, byte_count %d)", process, type, byte_count);
    process->array.is_inside_array = true;
    process->array.current_offset = 0;
    process->array.type = type;
    process->array.byte_count = byte_count;
}

static inline void end_array(cbe_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    process->array.is_inside_array = false;
    process->array.current_offset = 0;
    process->array.byte_count = 0;
}

static inline void add_array_length_field(cbe_encode_process* const process, const int64_t length)
{
    KSLOG_DEBUG("(process %p, length %d)", process, length);

    if(length <= MAX_VALUE_6_BIT)
    {
        add_primitive_uint_8(process, (uint8_t)((length << 2) | ARRAY_LENGTH_FIELD_WIDTH_6_BIT));
        return;
    }
    if(length <= MAX_VALUE_14_BIT)
    {
        add_primitive_uint_16(process, (uint16_t)((length << 2) | ARRAY_LENGTH_FIELD_WIDTH_14_BIT));
        return;
    }
    if(length <= MAX_VALUE_30_BIT)
    {
        add_primitive_uint_32(process, (uint32_t)((length << 2) | ARRAY_LENGTH_FIELD_WIDTH_30_BIT));
        return;
    }
    add_primitive_uint_64(process, (length << 2) | ARRAY_LENGTH_FIELD_WIDTH_62_BIT);
}

static inline cbe_encode_status add_int_small(cbe_encode_process* const process, const int8_t value)
{
    KSLOG_DEBUG("(process %p, value %d)", process, value);

    STOP_AND_EXIT_IF_IS_INSIDE_ARRAY(process);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM_WITH_TYPE(process, 0);

    add_primitive_int_8(process, value);
    swap_map_key_value_status(process);

    return CBE_ENCODE_STATUS_OK;
}

#define DEFINE_ADD_SCALAR_FUNCTION(DATA_TYPE, DEFINITION_TYPE, CBE_TYPE) \
    static inline cbe_encode_status add_ ## DEFINITION_TYPE(cbe_encode_process* const process, const DATA_TYPE value) \
    { \
        KSLOG_DEBUG("(process %p)", process); \
    \
        STOP_AND_EXIT_IF_IS_INSIDE_ARRAY(process); \
        STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM_WITH_TYPE(process, sizeof(value)); \
    \
        add_primitive_type(process, CBE_TYPE); \
        add_primitive_ ## DEFINITION_TYPE(process, value); \
    \
        swap_map_key_value_status(process); \
    \
        return CBE_ENCODE_STATUS_OK; \
    }
DEFINE_ADD_SCALAR_FUNCTION(int16_t,     int_16,      TYPE_INT_16)
DEFINE_ADD_SCALAR_FUNCTION(int32_t,     int_32,      TYPE_INT_32)
DEFINE_ADD_SCALAR_FUNCTION(int64_t,     int_64,      TYPE_INT_64)
DEFINE_ADD_SCALAR_FUNCTION(__int128,    int_128,     TYPE_INT_128)
DEFINE_ADD_SCALAR_FUNCTION(float,       float_32,    TYPE_FLOAT_32)
DEFINE_ADD_SCALAR_FUNCTION(double,      float_64,    TYPE_FLOAT_64)
DEFINE_ADD_SCALAR_FUNCTION(__float128,  float_128,   TYPE_FLOAT_128)
DEFINE_ADD_SCALAR_FUNCTION(_Decimal32,  decimal_32,  TYPE_DECIMAL_32)
DEFINE_ADD_SCALAR_FUNCTION(_Decimal64,  decimal_64,  TYPE_DECIMAL_64)
DEFINE_ADD_SCALAR_FUNCTION(_Decimal128, decimal_128, TYPE_DECIMAL_128)
DEFINE_ADD_SCALAR_FUNCTION(smalltime,   time,        TYPE_TIME)

static inline cbe_encode_status encode_string_header(cbe_encode_process* const process,
                                                     const int64_t byte_count,
                                                     const bool should_reserve_payload)
{
    KSLOG_DEBUG("(process %p, byte_count %d, should_reserve_payload %d)",
        process, byte_count, should_reserve_payload);

    cbe_type_field type = 0;
    int reserved_count = 0;

    if(byte_count > 15)
    {
        type = TYPE_STRING;
        reserved_count = get_array_length_field_width(byte_count);
    }
    else
    {
        type = TYPE_STRING_0 + byte_count;
    }

    if(should_reserve_payload)
    {
        reserved_count += byte_count;
    }

    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM_WITH_TYPE(process, reserved_count);

    add_primitive_type(process, type);
    if(byte_count > 15)
    {
        add_array_length_field(process, byte_count);
    }
    begin_array(process, ARRAY_TYPE_STRING, byte_count);

    swap_map_key_value_status(process);

    return CBE_ENCODE_STATUS_OK;
}

static cbe_encode_status encode_array_contents(cbe_encode_process* const process, 
                                               const uint8_t* start,
                                               const int64_t byte_count)
{
    KSLOG_DEBUG("(process %p, start %p, byte_count %d)", process, start, byte_count);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, 0);

    const int64_t space_in_buffer = get_remaining_space_in_buffer(process);
    const int64_t bytes_to_copy = minimum_int64(byte_count, space_in_buffer);

    if(process->array.type == ARRAY_TYPE_COMMENT)
    {
        if(!cbe_validate_comment(start, bytes_to_copy))
        {
            return CBE_ENCODE_ERROR_INVALID_DATA;
        }
    }

    if(process->array.type == ARRAY_TYPE_STRING)
    {
        if(!cbe_validate_string(start, bytes_to_copy))
        {
            return CBE_ENCODE_ERROR_INVALID_DATA;
        }
    }

    add_primitive_bytes(process, start, bytes_to_copy);
    process->array.current_offset += bytes_to_copy;

    KSLOG_DEBUG("Streamed %d bytes into array", bytes_to_copy);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, byte_count - space_in_buffer);

    if(process->array.current_offset == process->array.byte_count)
    {
        KSLOG_DEBUG("Array has ended");
        end_array(process);
    }

    return CBE_ENCODE_STATUS_OK;
}

// ===
// API
// ===

int cbe_encode_process_size(const int max_container_depth)
{
    KSLOG_TRACE("(max_container_depth %d)", max_container_depth)
    return sizeof(cbe_encode_process) + get_max_container_depth_or_default(max_container_depth);
}


cbe_encode_status cbe_encode_begin(struct cbe_encode_process* const process,
                                   uint8_t* const document_buffer,
                                   const int64_t byte_count,
                                   const int max_container_depth)
{
    KSLOG_TRACE("(process %p, document_buffer %p, byte_count %d, max_container_depth %d)",
        process, document_buffer, byte_count, max_container_depth);
    unlikely_if(process == NULL || document_buffer == NULL || byte_count < 0)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    zero_memory(process, sizeof(*process) + 1);
    cbe_encode_status status = cbe_encode_set_buffer(process, document_buffer, byte_count);
    unlikely_if(status != CBE_ENCODE_STATUS_OK)
    {
        return status;
    }
    process->container.max_depth = get_max_container_depth_or_default(max_container_depth);

    return status;
}

cbe_encode_status cbe_encode_set_buffer(cbe_encode_process* const process,
                                        uint8_t* const document_buffer,
                                        const int64_t byte_count)
{
    KSLOG_DEBUG("(process %p, document_buffer %p, byte_count %d)",
        process, document_buffer, byte_count);
    unlikely_if(process == NULL || document_buffer == NULL || byte_count < 0)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    process->buffer.start = document_buffer;
    process->buffer.position = document_buffer;
    process->buffer.end = document_buffer + byte_count;

    return CBE_ENCODE_STATUS_OK;
}

int64_t cbe_encode_get_buffer_offset(cbe_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    return process->buffer.position - process->buffer.start;
}

int64_t cbe_encode_get_array_offset(cbe_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    return process->array.current_offset;
}

int cbe_encode_get_container_level(cbe_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    return process->container.level;    
}

cbe_encode_status cbe_encode_add_padding(cbe_encode_process* const process, const int byte_count)
{
    KSLOG_DEBUG("(process %p, byte_count %d)", process, byte_count);
    unlikely_if(process == NULL || byte_count < 0)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    STOP_AND_EXIT_IF_IS_INSIDE_ARRAY(process);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM_WITH_TYPE(process, byte_count);

    for(int i = 0; i < byte_count; i++)
    {
        add_primitive_uint_8(process, TYPE_PADDING);
    }

    return CBE_ENCODE_STATUS_OK;
}

cbe_encode_status cbe_encode_add_nil(cbe_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    unlikely_if(process == NULL)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    STOP_AND_EXIT_IF_IS_WRONG_MAP_KEY_TYPE(process);
    STOP_AND_EXIT_IF_IS_INSIDE_ARRAY(process);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM_WITH_TYPE(process, 0);

    add_primitive_type(process, TYPE_EMPTY);
    swap_map_key_value_status(process);

    return CBE_ENCODE_STATUS_OK;
}

cbe_encode_status cbe_encode_add_boolean(cbe_encode_process* const process, const bool value)
{
    KSLOG_DEBUG("(process %p, value %d)", process, value);
    unlikely_if(process == NULL)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    STOP_AND_EXIT_IF_IS_INSIDE_ARRAY(process);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM_WITH_TYPE(process, 0);

    add_primitive_type(process, value ? TYPE_TRUE : TYPE_FALSE);
    swap_map_key_value_status(process);

    return CBE_ENCODE_STATUS_OK;
}

cbe_encode_status cbe_encode_add_int(cbe_encode_process* const process, const int value)
{
    KSLOG_DEBUG("(process %p, value %d)", process, value);
    unlikely_if(process == NULL)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    if(FITS_IN_INT_SMALL(value)) return add_int_small(process, value);
    if(FITS_IN_INT_16(value)) return add_int_16(process, value);
    if(FITS_IN_INT_32(value)) return add_int_32(process, value);
    if(FITS_IN_INT_64(value)) return add_int_64(process, value);
    return add_int_128(process, value);
}

cbe_encode_status cbe_encode_add_int_8(cbe_encode_process* const process, int8_t const value)
{
    KSLOG_DEBUG("(process %p, value %d)", process, value);
    unlikely_if(process == NULL)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    if(FITS_IN_INT_SMALL(value)) return add_int_small(process, value);
    return add_int_16(process, value);
}

cbe_encode_status cbe_encode_add_int_16(cbe_encode_process* const process, int16_t const value)
{
    KSLOG_DEBUG("(process %p, value %d)", process, value);
    unlikely_if(process == NULL)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    if(FITS_IN_INT_SMALL(value)) return add_int_small(process, value);
    return add_int_16(process, value);
}

cbe_encode_status cbe_encode_add_int_32(cbe_encode_process* const process, int32_t const value)
{
    KSLOG_DEBUG("(process %p, value %d)", process, value);
    unlikely_if(process == NULL)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    if(FITS_IN_INT_SMALL(value)) return add_int_small(process, value);
    if(FITS_IN_INT_16(value)) return add_int_16(process, value);
    return add_int_32(process, value);
}

cbe_encode_status cbe_encode_add_int_64(cbe_encode_process* const process, int64_t const value)
{
    KSLOG_DEBUG("(process %p, value %ld)", process, value);
    unlikely_if(process == NULL)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    if(FITS_IN_INT_SMALL(value)) return add_int_small(process, value);
    if(FITS_IN_INT_16(value)) return add_int_16(process, value);
    if(FITS_IN_INT_32(value)) return add_int_32(process, value);
    return add_int_64(process, value);
}

cbe_encode_status cbe_encode_add_int_128(cbe_encode_process* const process, const __int128 value)
{
    KSLOG_DEBUG("(process %p, value 0x%016x%016x)",
        process, (int64_t)(value>>64), (uint64_t)value);
    unlikely_if(process == NULL)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    if(FITS_IN_INT_SMALL(value)) return add_int_small(process, value);
    if(FITS_IN_INT_16(value)) return add_int_16(process, value);
    if(FITS_IN_INT_32(value)) return add_int_32(process, value);
    if(FITS_IN_INT_64(value)) return add_int_64(process, value);
    return add_int_128(process, value);
}

cbe_encode_status cbe_encode_add_float_32(cbe_encode_process* const process, const float value)
{
    KSLOG_DEBUG("(process %p, value %f)", process, (double)value);
    unlikely_if(process == NULL)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    return add_float_32(process, value);
}

cbe_encode_status cbe_encode_add_float_64(cbe_encode_process* const process, const double value)
{
    KSLOG_DEBUG("(process %p, value %f)", process, value);
    unlikely_if(process == NULL)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    if(FITS_IN_FLOAT_32(value)) return add_float_32(process, value);
    return add_float_64(process, value);
}

cbe_encode_status cbe_encode_add_float_128(cbe_encode_process* const process, const __float128 value)
{
    KSLOG_DEBUG("(process %p, value ~%d)", process, (double)value);
    unlikely_if(process == NULL)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    if(FITS_IN_FLOAT_32(value)) return add_float_32(process, value);
    if(FITS_IN_FLOAT_64(value)) return add_float_64(process, value);
    return add_float_128(process, value);
}

cbe_encode_status cbe_encode_add_decimal_32(cbe_encode_process* const process, const _Decimal32 value)
{
    KSLOG_DEBUG("(process %p, value ~%d)", process, (double)value);
    unlikely_if(process == NULL)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    return add_decimal_32(process, value);
}

cbe_encode_status cbe_encode_add_decimal_64(cbe_encode_process* const process, const _Decimal64 value)
{
    KSLOG_DEBUG("(process %p, value ~%d)", process, (double)value);
    unlikely_if(process == NULL)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    if(FITS_IN_DECIMAL_32(value)) return add_decimal_32(process, value);
    return add_decimal_64(process, value);
}

cbe_encode_status cbe_encode_add_decimal_128(cbe_encode_process* const process, const _Decimal128 value)
{
    KSLOG_DEBUG("(process %p, value ~%d)", process, (double)value);
    unlikely_if(process == NULL)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    if(FITS_IN_DECIMAL_32(value)) return add_decimal_32(process, value);
    if(FITS_IN_DECIMAL_64(value)) return add_decimal_64(process, value);
    return add_decimal_128(process, value);
}

cbe_encode_status cbe_encode_add_time(cbe_encode_process* const process, const smalltime value)
{
    KSLOG_DEBUG("(process %p, value %016x)", process, value);
    unlikely_if(process == NULL)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    return add_time(process, value);
}

cbe_encode_status cbe_encode_list_begin(cbe_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    unlikely_if(process == NULL)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    STOP_AND_EXIT_IF_IS_INSIDE_ARRAY(process);
    STOP_AND_EXIT_IF_IS_WRONG_MAP_KEY_TYPE(process);
    STOP_AND_EXIT_IF_MAX_CONTAINER_DEPTH_EXCEEDED(process);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM_WITH_TYPE(process, 0);

    add_primitive_type(process, TYPE_LIST);
    swap_map_key_value_status(process);

    process->container.level++;
    process->container.is_inside_map[process->container.level] = false;
    process->container.next_object_is_map_key = false;

    return CBE_ENCODE_STATUS_OK;
}

cbe_encode_status cbe_encode_map_begin(cbe_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    unlikely_if(process == NULL)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    STOP_AND_EXIT_IF_IS_INSIDE_ARRAY(process);
    STOP_AND_EXIT_IF_IS_WRONG_MAP_KEY_TYPE(process);
    STOP_AND_EXIT_IF_MAX_CONTAINER_DEPTH_EXCEEDED(process);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM_WITH_TYPE(process, 0);

    add_primitive_type(process, TYPE_MAP);
    swap_map_key_value_status(process);

    process->container.level++;
    process->container.is_inside_map[process->container.level] = true;
    process->container.next_object_is_map_key = true;

    return CBE_ENCODE_STATUS_OK;
}

cbe_encode_status cbe_encode_container_end(cbe_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    unlikely_if(process == NULL)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    STOP_AND_EXIT_IF_IS_INSIDE_ARRAY(process);
    STOP_AND_EXIT_IF_IS_NOT_INSIDE_CONTAINER(process);
    STOP_AND_EXIT_IF_MAP_VALUE_MISSING(process);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM_WITH_TYPE(process, 0);

    add_primitive_type(process, TYPE_END_CONTAINER);
    process->container.level--;
    process->container.next_object_is_map_key = process->container.is_inside_map[process->container.level];

    return CBE_ENCODE_STATUS_OK;
}

cbe_encode_status cbe_encode_binary_begin(cbe_encode_process* const process, const int64_t byte_count)
{
    KSLOG_DEBUG("(process %p, byte_count %d)", process, byte_count);
    unlikely_if(process == NULL || byte_count < 0)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    STOP_AND_EXIT_IF_IS_INSIDE_ARRAY(process);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM_WITH_TYPE(process, get_array_length_field_width(byte_count));

    add_primitive_type(process, TYPE_BINARY_DATA);
    add_array_length_field(process, byte_count);
    begin_array(process, ARRAY_TYPE_BINARY, byte_count);
    swap_map_key_value_status(process);

    return CBE_ENCODE_STATUS_OK;
}

cbe_encode_status cbe_encode_string_begin(cbe_encode_process* const process, const int64_t byte_count)
{
    KSLOG_DEBUG("(process %p, byte_count %d)", process, byte_count);
    unlikely_if(process == NULL || byte_count < 0)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    STOP_AND_EXIT_IF_IS_INSIDE_ARRAY(process);

    const bool should_reserve_payload = false;
    return encode_string_header(process, byte_count, should_reserve_payload);
}

cbe_encode_status cbe_encode_comment_begin(cbe_encode_process* const process, const int64_t byte_count)
{
    KSLOG_DEBUG("(process %p, byte_count %d)", process, byte_count);
    unlikely_if(process == NULL || byte_count < 0)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    STOP_AND_EXIT_IF_IS_INSIDE_ARRAY(process);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM_WITH_TYPE(process, get_array_length_field_width(byte_count));

    add_primitive_type(process, TYPE_COMMENT);
    add_array_length_field(process, byte_count);
    begin_array(process, ARRAY_TYPE_COMMENT, byte_count);
    swap_map_key_value_status(process);

    return CBE_ENCODE_STATUS_OK;
}

cbe_encode_status cbe_encode_add_data(cbe_encode_process* const process,
                                      const uint8_t* const start,
                                      const int64_t byte_count)
{
    KSLOG_DEBUG("(process %p, start %p, byte_count %d)", process, start, byte_count);
    unlikely_if(process == NULL || start == NULL || byte_count < 0)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }
    KSLOG_DATA_TRACE(start, byte_count, NULL);

    STOP_AND_EXIT_IF_IS_NOT_INSIDE_ARRAY(process);
    STOP_AND_EXIT_IF_ARRAY_LENGTH_EXCEEDED(process);

    return encode_array_contents(process, (const uint8_t*)start, byte_count);
}

cbe_encode_status cbe_encode_add_string(cbe_encode_process* const process, const char* const str)
{
    KSLOG_DEBUG("(process %p, str %p)", process, str);
    unlikely_if(process == NULL || str == NULL)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    KSLOG_DEBUG("Length: %d", strlen(str));
    KSLOG_DATA_TRACE(str, strlen(str), NULL);

    STOP_AND_EXIT_IF_IS_INSIDE_ARRAY(process);

    const bool should_reserve_payload = true;
    const int64_t byte_count = strlen(str);

    cbe_encode_status status = encode_string_header(process, byte_count, should_reserve_payload);
    unlikely_if(status != CBE_ENCODE_STATUS_OK)
    {
        return status;
    }

    return encode_array_contents(process, (const uint8_t*)str, byte_count);
}

cbe_encode_status cbe_encode_end(cbe_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    unlikely_if(process == NULL)
    {
        return CBE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    STOP_AND_EXIT_IF_IS_INSIDE_CONTAINER(process);
    STOP_AND_EXIT_IF_IS_INSIDE_ARRAY(process);

    KSLOG_DEBUG("Process ended successfully");
    return CBE_ENCODE_STATUS_OK;
}
