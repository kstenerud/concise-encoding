#include <memory.h>
#include <stdlib.h>
#include "cbe_internal.h"

// #define KSLogger_LocalLevel TRACE
#include "kslogger.h"

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
        int64_t current_offset;
        int64_t byte_count;
    } array;
    struct
    {
        int level;
        bool next_object_is_map_key;
        bool is_inside_map[MAX_CONTAINER_DEPTH];
    } container;
};
typedef struct cbe_encode_process cbe_encode_process;

#define STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(PROCESS, REQUIRED_BYTES) \
    if(get_remaining_space_in_buffer(PROCESS) < (int64_t)(REQUIRED_BYTES)) \
    { \
        KSLOG_DEBUG("STOP_AND_EXIT: Require %d bytes but only %d available.", (REQUIRED_BYTES), get_remaining_space_in_buffer(PROCESS)); \
        return CBE_ENCODE_STATUS_NEED_MORE_ROOM; \
    }

#define STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM_WITH_TYPE(PROCESS, REQUIRED_BYTES) \
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(PROCESS, ((REQUIRED_BYTES) + sizeof(cbe_encoded_type_field)))

#define STOP_AND_EXIT_IF_MAX_DEPTH_EXCEEDED(PROCESS) \
    if((PROCESS)->container.level + 1 >= MAX_CONTAINER_DEPTH) \
    { \
        KSLOG_DEBUG("STOP_AND_EXIT: Max depth %d exceeded", MAX_CONTAINER_DEPTH); \
        return CBE_ENCODE_ERROR_MAX_CONTAINER_DEPTH_EXCEEDED; \
    }

#define STOP_AND_EXIT_IF_MAP_VALUE_MISSING(PROCESS) \
    if((PROCESS)->container.is_inside_map[(PROCESS)->container.level] && !(PROCESS)->container.next_object_is_map_key) \
    { \
        KSLOG_DEBUG("STOP_AND_EXIT: No map value provided for last key"); \
        return CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY; \
    }

#define STOP_AND_EXIT_IF_EXPECTING_MAP_KEY(PROCESS) \
    if((PROCESS)->container.is_inside_map[process->container.level] && (PROCESS)->container.next_object_is_map_key) \
    { \
        KSLOG_DEBUG("STOP_AND_EXIT: Map key has an invalid type"); \
        return CBE_ENCODE_ERROR_INCORRECT_KEY_TYPE; \
    }

#define STOP_AND_EXIT_IF_INSIDE_ARRAY(PROCESS) \
    if((PROCESS)->array.is_inside_array) \
    { \
        KSLOG_DEBUG("STOP_AND_EXIT: We're inside an array when we shouldn't be"); \
        return CBE_ENCODE_ERROR_INCOMPLETE_FIELD; \
    }

#define FITS_IN_INT_SMALL(VALUE)  ((VALUE) >= TYPE_SMALLINT_MIN && (VALUE) <= TYPE_SMALLINT_MAX)
#define FITS_IN_INT_8(VALUE)      ((VALUE) == (int8_t)(VALUE))
#define FITS_IN_INT_16(VALUE)     ((VALUE) == (int16_t)(VALUE))
#define FITS_IN_INT_32(VALUE)     ((VALUE) == (int32_t)(VALUE))
#define FITS_IN_INT_64(VALUE)     ((VALUE) == (int64_t)(VALUE))
#define FITS_IN_FLOAT_32(VALUE)   ((VALUE) == (double)(float)(VALUE))
#define FITS_IN_FLOAT_64(VALUE)   ((VALUE) == (double)(VALUE))
#define FITS_IN_DECIMAL_32(VALUE) ((VALUE) == (_Decimal32)(VALUE))
#define FITS_IN_DECIMAL_64(VALUE) ((VALUE) == (_Decimal64)(VALUE))

static inline int64_t get_remaining_space_in_buffer(cbe_encode_process* process)
{
    return process->buffer.end - process->buffer.position;
}

static inline void swap_map_key_value_status(cbe_encode_process* process)
{
    process->container.next_object_is_map_key = !process->container.next_object_is_map_key;
}

static inline unsigned int get_length_field_width(const uint64_t length)
{
    if(length <= MAX_VALUE_6_BIT)  return sizeof(uint8_t);
    if(length <= MAX_VALUE_14_BIT) return sizeof(uint16_t);
    if(length <= MAX_VALUE_30_BIT) return sizeof(uint32_t);
    return sizeof(uint64_t);
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
        /* Must clear memory first because the compiler may do a partial store where there are zero bytes in the source */ \
        memset(process->buffer.position, 0, sizeof(value)); \
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
                                       const uint64_t byte_count)
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

static inline void add_length_field(cbe_encode_process* const process, const uint64_t length)
{
    KSLOG_DEBUG("Length: %d", length);
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

static inline cbe_encode_status add_small(cbe_encode_process* const process, const int8_t value)
{
    KSLOG_DEBUG(NULL);
    STOP_AND_EXIT_IF_INSIDE_ARRAY(process);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM_WITH_TYPE(process, 0);
    add_primitive_int_8(process, value);
    swap_map_key_value_status(process);
    return CBE_ENCODE_STATUS_OK;
}

#define DEFINE_ADD_SCALAR_FUNCTION(DATA_TYPE, DEFINITION_TYPE, CBE_TYPE) \
    static inline cbe_encode_status add_ ## DEFINITION_TYPE(cbe_encode_process* const process, const DATA_TYPE value) \
    { \
        KSLOG_DEBUG(NULL); \
        STOP_AND_EXIT_IF_INSIDE_ARRAY(process); \
        STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM_WITH_TYPE(process, sizeof(value)); \
        add_primitive_type(process, CBE_TYPE); \
        add_primitive_ ## DEFINITION_TYPE(process, value); \
        swap_map_key_value_status(process); \
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

static cbe_encode_status encode_string_header(cbe_encode_process* process,
                                              const int64_t byte_count,
                                              const bool should_reserve_payload)
{
    KSLOG_DEBUG("Length: %ld, Should reserve: %d", byte_count, should_reserve_payload);
    cbe_type_field type = 0;
    int reserved_count = 0;
    if(byte_count > 15)
    {
        type = TYPE_STRING;
        reserved_count = get_length_field_width(byte_count);
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
        add_length_field(process, byte_count);
    }
    process->array.is_inside_array = true;
    process->array.current_offset = 0;
    process->array.byte_count = byte_count;
    swap_map_key_value_status(process);
    return CBE_ENCODE_STATUS_OK;
}


cbe_encode_process* cbe_encode_begin(uint8_t* const document_buffer, const int64_t byte_count)
{
    KSLOG_DEBUG(NULL);
    cbe_encode_process* process = malloc(sizeof(*process));
    memset(process, 0, sizeof(*process));
    cbe_encode_set_buffer(process, document_buffer, byte_count);
    return process;
}

void cbe_encode_set_buffer(cbe_encode_process* const process,
                           uint8_t* const document_buffer,
                           const int64_t byte_count)
{
    KSLOG_DEBUG("Length: %d", byte_count);
    process->buffer.start = document_buffer;
    process->buffer.position = document_buffer;
    process->buffer.end = document_buffer + byte_count;
}

int64_t cbe_encode_get_buffer_offset(cbe_encode_process* const process)
{
    return process->buffer.position - process->buffer.start;
}

int64_t cbe_encode_get_array_offset(struct cbe_encode_process* process)
{
    return process->array.current_offset;
}

int cbe_encode_get_container_level(cbe_encode_process* const process)
{
    return process->container.level;    
}

cbe_encode_status cbe_encode_end(cbe_encode_process* const process)
{
    int container_level = cbe_encode_get_container_level(process);
    free(process);
    if(container_level != 0)
    {
        KSLOG_DEBUG("STOP_AND_EXIT: Ended encoding without closing all containers");
        return CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS;
    }
    KSLOG_DEBUG("Process ended successfully");
    return CBE_ENCODE_STATUS_OK;
}

cbe_encode_status cbe_encode_add_padding(cbe_encode_process* const process, int byte_count)
{
    STOP_AND_EXIT_IF_INSIDE_ARRAY(process);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM_WITH_TYPE(process, byte_count);
    for(int i = 0; i < byte_count; i++)
    {
        add_primitive_uint_8(process, TYPE_PADDING);
    }
    return CBE_ENCODE_STATUS_OK;
}

cbe_encode_status cbe_encode_add_empty(cbe_encode_process* const process)
{
    KSLOG_DEBUG(NULL);
    STOP_AND_EXIT_IF_EXPECTING_MAP_KEY(process);
    STOP_AND_EXIT_IF_INSIDE_ARRAY(process);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM_WITH_TYPE(process, 0);
    add_primitive_type(process, TYPE_EMPTY);
    swap_map_key_value_status(process);
    return CBE_ENCODE_STATUS_OK;
}

cbe_encode_status cbe_encode_add_boolean(cbe_encode_process* const process, const bool value)
{
    KSLOG_DEBUG("Value: %d", value);
    STOP_AND_EXIT_IF_INSIDE_ARRAY(process);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM_WITH_TYPE(process, 0);
    add_primitive_type(process, value ? TYPE_TRUE : TYPE_FALSE);
    swap_map_key_value_status(process);
    return CBE_ENCODE_STATUS_OK;
}

cbe_encode_status cbe_encode_add_int(cbe_encode_process* const process, const int value)
{
    KSLOG_DEBUG("Value: %d", value);
    if(FITS_IN_INT_SMALL(value)) return add_small(process, value);
    if(FITS_IN_INT_16(value)) return add_int_16(process, value);
    if(FITS_IN_INT_32(value)) return add_int_32(process, value);
    if(FITS_IN_INT_64(value)) return add_int_64(process, value);
    return add_int_128(process, value);
}

cbe_encode_status cbe_encode_add_int_8(cbe_encode_process* const process, int8_t const value)
{
    KSLOG_DEBUG("Value: %d", value);
    if(FITS_IN_INT_SMALL(value)) return add_small(process, value);
    return add_int_16(process, value);
}

cbe_encode_status cbe_encode_add_int_16(cbe_encode_process* const process, int16_t const value)
{
    KSLOG_DEBUG("Value: %d", value);
    if(FITS_IN_INT_SMALL(value)) return add_small(process, value);
    return add_int_16(process, value);
}

cbe_encode_status cbe_encode_add_int_32(cbe_encode_process* const process, int32_t const value)
{
    KSLOG_DEBUG("Value: %d", value);
    if(FITS_IN_INT_SMALL(value)) return add_small(process, value);
    if(FITS_IN_INT_16(value)) return add_int_16(process, value);
    return add_int_32(process, value);
}

cbe_encode_status cbe_encode_add_int_64(cbe_encode_process* const process, int64_t const value)
{
    KSLOG_DEBUG("Value: %d", value);
    if(FITS_IN_INT_SMALL(value)) return add_small(process, value);
    if(FITS_IN_INT_16(value)) return add_int_16(process, value);
    if(FITS_IN_INT_32(value)) return add_int_32(process, value);
    return add_int_64(process, value);
}

cbe_encode_status cbe_encode_add_int_128(cbe_encode_process* const process, const __int128 value)
{
    KSLOG_DEBUG("Value: TODO");
    if(FITS_IN_INT_SMALL(value)) return add_small(process, value);
    if(FITS_IN_INT_16(value)) return add_int_16(process, value);
    if(FITS_IN_INT_32(value)) return add_int_32(process, value);
    if(FITS_IN_INT_64(value)) return add_int_64(process, value);
    return add_int_128(process, value);
}

cbe_encode_status cbe_encode_add_float_32(cbe_encode_process* const process, const float value)
{
    KSLOG_DEBUG("Value: %f", (double)value);
    return add_float_32(process, value);
}

cbe_encode_status cbe_encode_add_float_64(cbe_encode_process* const process, const double value)
{
    KSLOG_DEBUG("Value: %f", value);
    if(FITS_IN_FLOAT_32(value)) return add_float_32(process, value);
    return add_float_64(process, value);
}

cbe_encode_status cbe_encode_add_float_128(cbe_encode_process* const process, const __float128 value)
{
    KSLOG_DEBUG("Value: TODO");
    if(FITS_IN_FLOAT_32(value)) return add_float_32(process, value);
    if(FITS_IN_FLOAT_64(value)) return add_float_64(process, value);
    return add_float_128(process, value);
}

cbe_encode_status cbe_encode_add_decimal_32(cbe_encode_process* const process, const _Decimal32 value)
{
    KSLOG_DEBUG("Value: TODO");
    return add_decimal_32(process, value);
}

cbe_encode_status cbe_encode_add_decimal_64(cbe_encode_process* const process, const _Decimal64 value)
{
    KSLOG_DEBUG("Value: TODO");
    if(FITS_IN_DECIMAL_32(value)) return add_decimal_32(process, value);
    return add_decimal_64(process, value);
}

cbe_encode_status cbe_encode_add_decimal_128(cbe_encode_process* const process, const _Decimal128 value)
{
    KSLOG_DEBUG("Value: TODO");
    if(FITS_IN_DECIMAL_32(value)) return add_decimal_32(process, value);
    if(FITS_IN_DECIMAL_64(value)) return add_decimal_64(process, value);
    return add_decimal_128(process, value);
}

cbe_encode_status cbe_encode_add_time(cbe_encode_process* const process, const smalltime value)
{
    KSLOG_DEBUG("Value: %016x", value);
    return add_time(process, value);
}

cbe_encode_status cbe_encode_begin_list(cbe_encode_process* const process)
{
    KSLOG_DEBUG(NULL);
    STOP_AND_EXIT_IF_INSIDE_ARRAY(process);
    STOP_AND_EXIT_IF_EXPECTING_MAP_KEY(process);
    STOP_AND_EXIT_IF_MAX_DEPTH_EXCEEDED(process);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM_WITH_TYPE(process, 0);
    add_primitive_type(process, TYPE_LIST);
    swap_map_key_value_status(process);
    process->container.level++;
    process->container.is_inside_map[process->container.level] = false;
    process->container.next_object_is_map_key = false;
    return CBE_ENCODE_STATUS_OK;
}

cbe_encode_status cbe_encode_begin_map(cbe_encode_process* const process)
{
    KSLOG_DEBUG(NULL);
    STOP_AND_EXIT_IF_INSIDE_ARRAY(process);
    STOP_AND_EXIT_IF_EXPECTING_MAP_KEY(process);
    STOP_AND_EXIT_IF_MAX_DEPTH_EXCEEDED(process);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM_WITH_TYPE(process, 0);
    add_primitive_type(process, TYPE_MAP);
    swap_map_key_value_status(process);
    process->container.level++;
    process->container.is_inside_map[process->container.level] = true;
    process->container.next_object_is_map_key = true;
    return CBE_ENCODE_STATUS_OK;
}

cbe_encode_status cbe_encode_end_container(cbe_encode_process* const process)
{
    KSLOG_DEBUG(NULL);
    if(process->container.level - 1 < 0)
    {
        KSLOG_DEBUG("STOP_AND_EXIT: Too many end-containers");
        return CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS;
    }
    STOP_AND_EXIT_IF_INSIDE_ARRAY(process);
    STOP_AND_EXIT_IF_MAP_VALUE_MISSING(process);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM_WITH_TYPE(process, 0);
    add_primitive_type(process, TYPE_END_CONTAINER);
    process->container.level--;
    process->container.next_object_is_map_key = process->container.is_inside_map[process->container.level];
    return CBE_ENCODE_STATUS_OK;
}

static cbe_encode_status encode_array_contents(cbe_encode_process* const process, 
                                               const uint8_t* start,
                                               const int64_t byte_count)
{
    int64_t bytes_to_copy = byte_count;
    int64_t bytes_remaining = get_remaining_space_in_buffer(process);
    if(bytes_remaining < bytes_to_copy)
    {
        if(bytes_remaining <= 0)
        {
            STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, byte_count);
        }
        bytes_to_copy = bytes_remaining;
    }
    add_primitive_bytes(process, start, bytes_to_copy);
    process->array.current_offset += bytes_to_copy;
    if(bytes_remaining < byte_count)
    {
        KSLOG_DEBUG("Encoded %d of %d bytes. Need more room.", bytes_to_copy, byte_count);
        return CBE_ENCODE_STATUS_NEED_MORE_ROOM;
    }
    KSLOG_DEBUG("Encoded %d bytes", bytes_to_copy);
    if(process->array.current_offset == process->array.byte_count)
    {
        KSLOG_DEBUG("Array has ended");
        process->array.is_inside_array = false;
    }
    return CBE_ENCODE_STATUS_OK;
}

cbe_encode_status cbe_encode_begin_binary(cbe_encode_process* const process, const int64_t byte_count)
{
    KSLOG_DEBUG("Length: %ld", byte_count);
    STOP_AND_EXIT_IF_INSIDE_ARRAY(process);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM_WITH_TYPE(process, get_length_field_width(byte_count));
    add_primitive_type(process, TYPE_BINARY_DATA);
    add_length_field(process, byte_count);
    process->array.is_inside_array = true;
    process->array.current_offset = 0;
    process->array.byte_count = byte_count;
    swap_map_key_value_status(process);
    return CBE_ENCODE_STATUS_OK;
}

cbe_encode_status cbe_encode_begin_string(cbe_encode_process* const process, const int64_t byte_count)
{
    KSLOG_DEBUG("Length: %ld", byte_count);
    STOP_AND_EXIT_IF_INSIDE_ARRAY(process);
    const bool should_reserve_payload = false;
    return encode_string_header(process, byte_count, should_reserve_payload);
}

cbe_encode_status cbe_encode_add_data(cbe_encode_process* const process,
                                      const uint8_t* const start,
                                      const int64_t byte_count)
{
    KSLOG_DEBUG("Length: %d", byte_count);
    KSLOG_DATA_TRACE(start, byte_count, NULL);
    if(!process->array.is_inside_array)
    {
        KSLOG_DEBUG("STOP_AND_EXIT: Attempted to add data while not inside an array field.");
        return CBE_ENCODE_ERROR_NOT_INSIDE_ARRAY_FIELD;
    }
    int64_t new_array_offset = process->array.current_offset + byte_count;
    if(new_array_offset > process->array.byte_count)
    {
        KSLOG_DEBUG("STOP_AND_EXIT: Attempted to write more bytes (%d) than are available in the array (%d)", new_array_offset, process->array.byte_count);
        return CBE_ENCODE_ERROR_FIELD_LENGTH_EXCEEDED;
    }
    return encode_array_contents(process, (const uint8_t*)start, byte_count);
}

cbe_encode_status cbe_encode_add_string(cbe_encode_process* const process, const char* const str)
{
    KSLOG_DEBUG("Length: %d", strlen(str));
    KSLOG_DATA_TRACE(str, strlen(str), NULL);
    STOP_AND_EXIT_IF_INSIDE_ARRAY(process);
    const bool should_reserve_payload = true;
    const int byte_count = strlen(str);
    cbe_encode_status status = encode_string_header(process, byte_count, should_reserve_payload);
    if(status != CBE_ENCODE_STATUS_OK)
    {
        return status;
    }
    return encode_array_contents(process, (const uint8_t*)str, byte_count);
}
