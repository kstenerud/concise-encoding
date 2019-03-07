#include "cte_internal.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"


// ====
// Data
// ====

struct cte_encode_process
{
    int indent_spaces;
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
        cte_binary_encoding_radix radix;
    } array;
    struct
    {
        int max_depth;
        int level;
        bool next_object_is_map_key;
        bool is_inside_map[];
    } container;
};
typedef struct cte_encode_process cte_encode_process;

// ==============
// Utility Macros
// ==============

#define likely_if(TEST_FOR_TRUTH) if(__builtin_expect(TEST_FOR_TRUTH, 1))
#define unlikely_if(TEST_FOR_TRUTH) if(__builtin_expect(TEST_FOR_TRUTH, 0))


// ==============
// Error Handlers
// ==============

#define STOP_AND_EXIT_IF_RESULT_STATUS_NOT_OK(PROCESS, ...) \
{ \
    cte_decode_status inner_status = __VA_ARGS__; \
    unlikely_if(inner_status != CTE_DECODE_STATUS_OK) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: API call returned non-ok value %d", inner_status); \
        return inner_status; \
    } \
}

#define STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(PROCESS, REQUIRED_BYTES) \
    unlikely_if(get_remaining_space_in_buffer(PROCESS) < (int64_t)(REQUIRED_BYTES)) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: Require %d bytes but only %d available.", \
            (REQUIRED_BYTES), get_remaining_space_in_buffer(PROCESS)); \
        return CTE_ENCODE_STATUS_NEED_MORE_ROOM; \
    }

#define STOP_AND_EXIT_IF_IS_INSIDE_ARRAY(PROCESS) \
    unlikely_if((PROCESS)->array.is_inside_array) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: We're inside an array when we shouldn't be"); \
        return CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD; \
    }

#define STOP_AND_EXIT_IF_IS_NOT_INSIDE_ARRAY(PROCESS) \
    unlikely_if(!(PROCESS)->array.is_inside_array) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: We're not inside an array when we should be"); \
        return CTE_ENCODE_ERROR_NOT_INSIDE_ARRAY_FIELD; \
    }

#define STOP_AND_EXIT_IF_MAX_CONTAINER_DEPTH_EXCEEDED(PROCESS) \
    unlikely_if((PROCESS)->container.level + 1 >= (PROCESS)->container.max_depth) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: Max depth %d exceeded", (PROCESS)->container.max_depth); \
        return CTE_ENCODE_ERROR_MAX_CONTAINER_DEPTH_EXCEEDED; \
    }

#define STOP_AND_EXIT_IF_IS_NOT_INSIDE_CONTAINER(PROCESS) \
    unlikely_if((PROCESS)->container.level <= 0) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: We're not inside a container"); \
        return CTE_ENCODE_ERROR_UNBALANCED_CONTAINERS; \
    }

#define STOP_AND_EXIT_IF_IS_INSIDE_CONTAINER(PROCESS) \
    unlikely_if((PROCESS)->container.level != 0) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: There are still open containers when there shouldn't be"); \
        return CTE_ENCODE_ERROR_UNBALANCED_CONTAINERS; \
    }

#define STOP_AND_EXIT_IF_MAP_VALUE_MISSING(PROCESS) \
    unlikely_if((PROCESS)->container.is_inside_map[(PROCESS)->container.level] && \
        !(PROCESS)->container.next_object_is_map_key) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: No map value provided for previous key"); \
        return CTE_ENCODE_ERROR_MAP_MISSING_VALUE_FOR_KEY; \
    }

#define STOP_AND_EXIT_IF_IS_WRONG_MAP_KEY_TYPE(PROCESS) \
    unlikely_if((PROCESS)->container.is_inside_map[process->container.level] && \
        (PROCESS)->container.next_object_is_map_key) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: Map key has an invalid type"); \
        return CTE_ENCODE_ERROR_INCORRECT_MAP_KEY_TYPE; \
    }


// =======
// Utility
// =======

static inline int64_t minimum_int64(const int64_t a, const int64_t b)
{
    return a < b ? a : b;
}

static inline int64_t get_remaining_space_in_buffer(cte_encode_process* const process)
{
    return process->buffer.end - process->buffer.position;
}

static inline void swap_map_key_value_status(cte_encode_process* const process)
{
    process->container.next_object_is_map_key = !process->container.next_object_is_map_key;
}

static inline void add_primitive_bytes(cte_encode_process* const process,
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

static inline void add_primitive_chars(cte_encode_process* const process,
                                       const char* const bytes,
                                       const int64_t byte_count)
{
    return add_primitive_bytes(process, (const uint8_t*)bytes, byte_count);
}

static inline void begin_array(cte_encode_process* const process, array_type type)
{
    KSLOG_DEBUG("(process %p, type %d, byte_count %d)", process, type);
    process->array.is_inside_array = true;
    process->array.current_offset = 0;
    process->array.type = type;
}

static inline void end_array(cte_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    process->array.is_inside_array = false;
    process->array.current_offset = 0;
}

static inline cte_encode_status add_encoded_object(cte_encode_process* const process,
                                                   const char* const object,
                                                   const int64_t byte_count)
{
    STOP_AND_EXIT_IF_IS_INSIDE_ARRAY(process);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, byte_count);
    swap_map_key_value_status(process);
    add_primitive_chars(process, object, byte_count);
    return CTE_ENCODE_STATUS_OK;
}

static inline cte_encode_status add_int_64(cte_encode_process* const process, const int64_t value)
{
    char buffer[100];
    sprintf(buffer, "%ld", value);
    return add_encoded_object(process, buffer, strlen(buffer));
}

static inline cte_encode_status add_float_64(cte_encode_process* const process, int precision, const double value)
{
    char fmt[10];
    sprintf(fmt, "%%.%dlg", precision);
    char buffer[100];
    sprintf(buffer, fmt, value);
    return add_encoded_object(process, buffer, strlen(buffer));
}

static cte_encode_status encode_array_contents(cte_encode_process* const process, 
                                               const uint8_t* const start,
                                               int64_t* const byte_count)
{
    KSLOG_DEBUG("(process %p, start %p, byte_count %d)", process, start, *byte_count);

    likely_if(*byte_count > 0)
    {
        const int64_t want_to_copy = *byte_count;
        const int64_t space_in_buffer = get_remaining_space_in_buffer(process);
        const int64_t bytes_to_copy = minimum_int64(want_to_copy, space_in_buffer);

        KSLOG_DEBUG("Type: %d", process->array.type);
        if(process->array.type == ARRAY_TYPE_COMMENT)
        {
            unlikely_if(!cte_validate_comment(start, bytes_to_copy))
            {
                KSLOG_DEBUG("invalid data");
                return CTE_ENCODE_ERROR_INVALID_ARRAY_DATA;
            }
        }

        if(process->array.type == ARRAY_TYPE_STRING)
        {
            unlikely_if(!cte_validate_string(start, bytes_to_copy))
            {
                KSLOG_DEBUG("invalid data");
                return CTE_ENCODE_ERROR_INVALID_ARRAY_DATA;
            }
        }

        add_primitive_bytes(process, start, bytes_to_copy);
        process->array.current_offset += bytes_to_copy;
        *byte_count = bytes_to_copy;

        KSLOG_DEBUG("Streamed %d bytes into array", bytes_to_copy);
        STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, want_to_copy - bytes_to_copy);
    }

    return CTE_ENCODE_STATUS_OK;
}

// ===
// API
// ===

int cte_encode_process_size(const int max_container_depth)
{
    KSLOG_TRACE("(max_container_depth %d)", max_container_depth)
    return sizeof(cte_encode_process) + get_max_container_depth_or_default(max_container_depth);
}


cte_encode_status cte_encode_begin(struct cte_encode_process* const process,
                                   uint8_t* const document_buffer,
                                   int64_t byte_count,
                                   int max_container_depth,
                                   int indent_spaces)
{
    KSLOG_TRACE("(process %p, document_buffer %p, byte_count %d, max_container_depth %d)",
        process, document_buffer, byte_count, max_container_depth);
    unlikely_if(process == NULL || document_buffer == NULL || byte_count < 0)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    zero_memory(process, sizeof(*process) + 1);
    cte_encode_status status = cte_encode_set_buffer(process, document_buffer, byte_count);
    unlikely_if(status != CTE_ENCODE_STATUS_OK)
    {
        return status;
    }
    process->container.max_depth = get_max_container_depth_or_default(max_container_depth);
    process->indent_spaces = indent_spaces;

    return status;
}

cte_encode_status cte_encode_set_buffer(cte_encode_process* const process,
                                        uint8_t* const document_buffer,
                                        const int64_t byte_count)
{
    KSLOG_DEBUG("(process %p, document_buffer %p, byte_count %d)",
        process, document_buffer, byte_count);
    unlikely_if(process == NULL || document_buffer == NULL || byte_count < 0)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    process->buffer.start = document_buffer;
    process->buffer.position = document_buffer;
    process->buffer.end = document_buffer + byte_count;

    return CTE_ENCODE_STATUS_OK;
}

int64_t cte_encode_get_buffer_offset(cte_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    return process->buffer.position - process->buffer.start;
}

int cte_encode_get_container_level(cte_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    return process->container.level;    
}

cte_encode_status cte_encode_add_nil(cte_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    STOP_AND_EXIT_IF_IS_WRONG_MAP_KEY_TYPE(process);

    return add_encoded_object(process, "nil", 3);
}

cte_encode_status cte_encode_add_boolean(cte_encode_process* const process, const bool value)
{
    KSLOG_DEBUG("(process %p, value %d)", process, value);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    if(value)
    {
        return add_encoded_object(process, "true", 4);
    }
    else
    {
        return add_encoded_object(process, "false", 5);
    }
}

cte_encode_status cte_encode_add_int(cte_encode_process* const process, const int value)
{
    KSLOG_DEBUG("(process %p, value %d)", process, value);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    return add_int_64(process, value);
}

cte_encode_status cte_encode_add_int_8(cte_encode_process* const process, int8_t const value)
{
    KSLOG_DEBUG("(process %p, value %d)", process, value);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    return add_int_64(process, value);
}

cte_encode_status cte_encode_add_int_16(cte_encode_process* const process, int16_t const value)
{
    KSLOG_DEBUG("(process %p, value %d)", process, value);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    return add_int_64(process, value);
}

cte_encode_status cte_encode_add_int_32(cte_encode_process* const process, int32_t const value)
{
    KSLOG_DEBUG("(process %p, value %d)", process, value);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    return add_int_64(process, value);
}

cte_encode_status cte_encode_add_int_64(cte_encode_process* const process, int64_t const value)
{
    KSLOG_DEBUG("(process %p, value %ld)", process, value);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    return add_int_64(process, value);
}

cte_encode_status cte_encode_add_int_128(cte_encode_process* const process, const __int128 value)
{
    KSLOG_DEBUG("(process %p, value 0x%016x%016x)",
        process, (int64_t)(value>>64), (uint64_t)value);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    // TODO
    return add_int_64(process, (int64_t)value);
}

cte_encode_status cte_encode_add_float_32(cte_encode_process* const process,
                                          const int precision,
                                          const float value)
{
    KSLOG_DEBUG("(process %p, value %f)", process, (double)value);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    return add_float_64(process, precision, value);
}

cte_encode_status cte_encode_add_float_64(cte_encode_process* const process,
                                          const int precision,
                                          const double value)
{
    KSLOG_DEBUG("(process %p, value %f)", process, value);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    return add_float_64(process, precision, value);
}

cte_encode_status cte_encode_add_float_128(cte_encode_process* const process,
                                           const int precision,
                                           const __float128 value)
{
    KSLOG_DEBUG("(process %p, value ~%d)", process, (double)value);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    // TODO
    return add_float_64(process, precision, (double)value);
}

cte_encode_status cte_encode_add_decimal_32(cte_encode_process* const process, const _Decimal32 value)
{
    KSLOG_DEBUG("(process %p, value ~%d)", process, (double)value);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    // TODO
    return add_float_64(process, 10, (double)value);
}

cte_encode_status cte_encode_add_decimal_64(cte_encode_process* const process, const _Decimal64 value)
{
    KSLOG_DEBUG("(process %p, value ~%d)", process, (double)value);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    // TODO
    return add_float_64(process, 10, (double)value);
}

cte_encode_status cte_encode_add_decimal_128(cte_encode_process* const process, const _Decimal128 value)
{
    KSLOG_DEBUG("(process %p, value ~%d)", process, (double)value);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    // TODO
    return add_float_64(process, 10, (double)value);
}

cte_encode_status cte_encode_add_time(cte_encode_process* const process, const smalltime value)
{
    KSLOG_DEBUG("(process %p, value %016x)", process, value);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    // TODO
    (void)value;
    return add_encoded_object(process, "TODO-time", 10);
}

cte_encode_status cte_encode_list_begin(cte_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    STOP_AND_EXIT_IF_IS_WRONG_MAP_KEY_TYPE(process);
    STOP_AND_EXIT_IF_MAX_CONTAINER_DEPTH_EXCEEDED(process);

    STOP_AND_EXIT_IF_RESULT_STATUS_NOT_OK(process, add_encoded_object(process, "[", 1));

    process->container.level++;
    process->container.is_inside_map[process->container.level] = false;
    process->container.next_object_is_map_key = false;

    return CTE_ENCODE_STATUS_OK;
}

cte_encode_status cte_encode_map_begin(cte_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    STOP_AND_EXIT_IF_IS_WRONG_MAP_KEY_TYPE(process);
    STOP_AND_EXIT_IF_MAX_CONTAINER_DEPTH_EXCEEDED(process);

    STOP_AND_EXIT_IF_RESULT_STATUS_NOT_OK(process, add_encoded_object(process, "{", 1));

    process->container.level++;
    process->container.is_inside_map[process->container.level] = true;
    process->container.next_object_is_map_key = true;

    return CTE_ENCODE_STATUS_OK;
}

cte_encode_status cte_encode_container_end(cte_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    STOP_AND_EXIT_IF_IS_INSIDE_ARRAY(process);
    STOP_AND_EXIT_IF_IS_NOT_INSIDE_CONTAINER(process);
    STOP_AND_EXIT_IF_MAP_VALUE_MISSING(process);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, 1);

    if(process->container.is_inside_map[process->container.level])
    {
        add_primitive_chars(process, "}", 1);
    }
    else
    {
        add_primitive_chars(process, "]", 1);
    }

    process->container.level--;
    process->container.next_object_is_map_key = process->container.is_inside_map[process->container.level];

    return CTE_ENCODE_STATUS_OK;
}

cte_encode_status cte_encode_binary_begin(cte_encode_process* const process,
                                          const cte_binary_encoding_radix radix)
{
    KSLOG_DEBUG("(process %p)", process);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    STOP_AND_EXIT_IF_RESULT_STATUS_NOT_OK(process, add_encoded_object(process, "b/", 2));
    begin_array(process, ARRAY_TYPE_BINARY);
    process->array.radix = radix;
    return CTE_ENCODE_STATUS_OK;
}

cte_encode_status cte_encode_string_begin(cte_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    STOP_AND_EXIT_IF_RESULT_STATUS_NOT_OK(process, add_encoded_object(process, "\"", 1));
    begin_array(process, ARRAY_TYPE_STRING);
    return CTE_ENCODE_STATUS_OK;
}

cte_encode_status cte_encode_comment_begin(cte_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    STOP_AND_EXIT_IF_RESULT_STATUS_NOT_OK(process, add_encoded_object(process, "#", 1));
    begin_array(process, ARRAY_TYPE_COMMENT);
    return CTE_ENCODE_STATUS_OK;
}

cte_encode_status cte_encode_add_data(cte_encode_process* const process,
                                      const uint8_t* const start,
                                      int64_t* const byte_count)
{
    KSLOG_DEBUG("(process %p, start %p, byte_count %d)",
        process, start, byte_count == NULL ? -123456789 : *byte_count);
    unlikely_if(process == NULL || byte_count == NULL || *byte_count < 0)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }
    unlikely_if(*byte_count == 0)
    {
        return CTE_ENCODE_STATUS_OK;
    }
    unlikely_if(start == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }
    KSLOG_DATA_TRACE(start, *byte_count, NULL);

    STOP_AND_EXIT_IF_IS_NOT_INSIDE_ARRAY(process);

    return encode_array_contents(process, start, byte_count);
}

cte_encode_status cte_encode_string_end(cte_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    add_primitive_chars(process, "\"", 1);
    end_array(process);
    return CTE_ENCODE_STATUS_OK;
}

cte_encode_status cte_encode_binary_end(cte_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    add_primitive_chars(process, "/", 1);
    end_array(process);
    return CTE_ENCODE_STATUS_OK;
}

cte_encode_status cte_encode_comment_end(cte_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    add_primitive_chars(process, "\n", 1);
    end_array(process);
    return CTE_ENCODE_STATUS_OK;
}

cte_encode_status cte_encode_add_string(cte_encode_process* const process,
                                        const char* const string_start,
                                        const int64_t byte_count)
{
    uint8_t* const last_position = process->buffer.position;
    STOP_AND_EXIT_IF_RESULT_STATUS_NOT_OK(process, cte_encode_string_begin(process));
    int64_t byte_count_copy = byte_count;
    cte_encode_status status = cte_encode_add_data(process, (const uint8_t*)string_start, &byte_count_copy);
    unlikely_if(status != CTE_ENCODE_STATUS_OK)
    {
        process->buffer.position = last_position;
    }
    return status;
}

cte_encode_status cte_encode_add_binary(cte_encode_process* const process,
                                        const cte_binary_encoding_radix radix,
                                        const uint8_t* const data,
                                        const int64_t byte_count)
{
    uint8_t* const last_position = process->buffer.position;
    STOP_AND_EXIT_IF_RESULT_STATUS_NOT_OK(process, cte_encode_binary_begin(process, radix));
    int64_t byte_count_copy = byte_count;
    cte_encode_status status = cte_encode_add_data(process, data, &byte_count_copy);
    unlikely_if(status != CTE_ENCODE_STATUS_OK)
    {
        process->buffer.position = last_position;
    }
    return status;
}

cte_encode_status cte_encode_add_comment(cte_encode_process* const process,
                                        const char* const comment_start,
                                        const int64_t byte_count)
{
    uint8_t* const last_position = process->buffer.position;
    STOP_AND_EXIT_IF_RESULT_STATUS_NOT_OK(process, cte_encode_comment_begin(process));
    int64_t byte_count_copy = byte_count;
    cte_encode_status status = cte_encode_add_data(process, (const uint8_t*)comment_start, &byte_count_copy);
    unlikely_if(status != CTE_ENCODE_STATUS_OK)
    {
        process->buffer.position = last_position;
    }
    return status;
}

cte_encode_status cte_encode_end(cte_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    unlikely_if(process == NULL)
    {
        return CTE_ENCODE_ERROR_INVALID_ARGUMENT;
    }

    STOP_AND_EXIT_IF_IS_INSIDE_CONTAINER(process);
    STOP_AND_EXIT_IF_IS_INSIDE_ARRAY(process);

    KSLOG_DEBUG("Process ended successfully");
    return CTE_ENCODE_STATUS_OK;
}
