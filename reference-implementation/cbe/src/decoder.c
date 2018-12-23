#include <memory.h>
#include <stdlib.h>
#include "cbe_internal.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"


// ====
// Data
// ====

struct cbe_decode_process
{
    const cbe_decode_callbacks* callbacks;
    void* user_context;
    struct
    {
        const uint8_t* start;
        const uint8_t* end;
        const uint8_t* position;
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
typedef struct cbe_decode_process cbe_decode_process;


// ==============
// Error Handlers
// ==============

#define STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(PROCESS, BYTE_COUNT) \
    if(get_remaining_space_in_buffer(PROCESS) < (int64_t)(BYTE_COUNT)) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: Require %d bytes but only %d available.", (BYTE_COUNT), get_remaining_space_in_buffer(PROCESS)); \
        return CBE_DECODE_STATUS_NEED_MORE_DATA; \
    }

#define STOP_AND_EXIT_IF_IS_INSIDE_ARRAY(PROCESS) \
    if((PROCESS)->array.is_inside_array) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: We're inside an array when we shouldn't be"); \
        return CBE_DECODE_ERROR_INCOMPLETE_FIELD; \
    }

#define STOP_AND_EXIT_IF_IS_INSIDE_CONTAINER(PROCESS) \
    if((PROCESS)->container.level != 0) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: There are still open containers when there shouldn't be"); \
        return CBE_DECODE_ERROR_UNBALANCED_CONTAINERS; \
    }

#define STOP_AND_EXIT_IF_MAP_VALUE_MISSING(PROCESS) \
    if((PROCESS)->container.level < MAX_CONTAINER_DEPTH) \
        if((PROCESS)->container.is_inside_map[(PROCESS)->container.level] && !(PROCESS)->container.next_object_is_map_key) \
        { \
            KSLOG_DEBUG("STOP AND EXIT: Missing value for previous key"); \
            return CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY; \
        }

#define STOP_AND_EXIT_IF_IS_WRONG_MAP_KEY_TYPE(PROCESS) \
    if((PROCESS)->container.level < MAX_CONTAINER_DEPTH) \
    if((PROCESS)->container.is_inside_map[(PROCESS)->container.level] && (PROCESS)->container.next_object_is_map_key) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: Map key has an invalid type"); \
        return CBE_DECODE_ERROR_INCORRECT_KEY_TYPE; \
    }

#define STOP_AND_EXIT_IF_DECODE_STATUS_NOT_OK(...) \
{ \
    cbe_decode_status inner_status = __VA_ARGS__; \
    if(inner_status != CBE_DECODE_STATUS_OK) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: API call returned non-ok value %d", inner_status); \
        return inner_status; \
    } \
}

#define STOP_AND_EXIT_IF_FAILED_CALLBACK(...) \
    if(!__VA_ARGS__) \
    { \
        KSLOG_DEBUG("STOP AND EXIT: Callback returned false"); \
        return CBE_DECODE_STATUS_STOPPED_IN_CALLBACK; \
    }


// =======
// Utility
// =======

static inline int64_t get_remaining_space_in_buffer(cbe_decode_process* process)
{
    return process->buffer.end - process->buffer.position;
}

static inline void swap_map_key_value_status(cbe_decode_process* process)
{
    process->container.next_object_is_map_key = !process->container.next_object_is_map_key;
}

static inline int64_t consume_bytes(cbe_decode_process* const process, int64_t byte_count)
{
    process->buffer.position += byte_count;
    return byte_count;
}

static inline uint8_t peek_uint_8(const cbe_decode_process* const process)
{
    return *process->buffer.position;
}

static inline uint8_t read_uint_8(cbe_decode_process* const process)
{
    KSLOG_DATA_DEBUG(process->buffer.position, 1, NULL);
    return *process->buffer.position++;
}

#define DEFINE_READ_FUNCTION(TYPE, TYPE_SUFFIX) \
static inline TYPE read_ ## TYPE_SUFFIX(cbe_decode_process* const process) \
{ \
    const safe_ ## TYPE_SUFFIX* const safe = (safe_ ## TYPE_SUFFIX*)process->buffer.position; \
    KSLOG_DATA_DEBUG(process->buffer.position, sizeof(*safe), NULL); \
    consume_bytes(process, sizeof(*safe)); \
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
DEFINE_READ_FUNCTION(__float128,  float_128)
DEFINE_READ_FUNCTION(_Decimal32,  decimal_32)
DEFINE_READ_FUNCTION(_Decimal64,  decimal_64)
DEFINE_READ_FUNCTION(_Decimal128, decimal_128)
DEFINE_READ_FUNCTION(smalltime,   time)

static inline int peek_array_length_field_width(const cbe_decode_process* const process)
{
    return 1 << (peek_uint_8(process) & 3);
}

static inline int64_t read_array_length(cbe_decode_process* const process)
{
    KSLOG_DEBUG(NULL);
    switch(peek_array_length_field_width(process))
    {
        case 1:  return (int64_t)(read_uint_8(process) >> 2);
        case 2:  return (int64_t)(read_uint_16(process) >> 2);
        case 4:  return (int64_t)(read_uint_32(process) >> 2);
        default: return (int64_t)(read_uint_64(process) >> 2);
    }
}

static inline cbe_decode_status begin_object(cbe_decode_process* process, const int64_t initial_byte_count)
{
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, initial_byte_count);
    return CBE_DECODE_STATUS_OK;
}

static inline cbe_decode_status begin_nonkeyable_object(cbe_decode_process* process, const int64_t initial_byte_count)
{
    STOP_AND_EXIT_IF_IS_WRONG_MAP_KEY_TYPE(process);
    return begin_object(process, initial_byte_count);
}

static inline void end_object(cbe_decode_process* process)
{
    swap_map_key_value_status(process);
}

static inline void internal_begin_array(cbe_decode_process* const process, int64_t byte_count)
{
    process->array.is_inside_array = true;
    process->array.current_offset = 0;
    process->array.byte_count = byte_count;
}

static cbe_decode_status begin_array(cbe_decode_process* const process,
                                     bool (*on_begin_array)(struct cbe_decode_process* process, int64_t byte_count))
{
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, 1);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, peek_array_length_field_width(process));
    int64_t array_byte_count = read_array_length(process);
    KSLOG_DEBUG("Length: %d", array_byte_count);
    STOP_AND_EXIT_IF_FAILED_CALLBACK(on_begin_array(process, array_byte_count));
    internal_begin_array(process, array_byte_count);
    return CBE_DECODE_STATUS_OK;
}

static cbe_decode_status stream_array(cbe_decode_process* const process)
{
    int64_t bytes_in_array = process->array.byte_count - process->array.current_offset;
    int64_t space_in_buffer = get_remaining_space_in_buffer(process);
    int64_t bytes_to_stream = bytes_in_array <= space_in_buffer ? bytes_in_array : space_in_buffer;
    KSLOG_DEBUG("Length: arr %d vs buf %d: %d bytes", bytes_in_array, space_in_buffer, bytes_to_stream);
    KSLOG_DATA_TRACE(process->buffer.position, bytes_to_stream, NULL);
    STOP_AND_EXIT_IF_FAILED_CALLBACK(process->callbacks->on_add_data(process, process->buffer.position, bytes_to_stream));
    consume_bytes(process, bytes_to_stream);
    process->array.current_offset += bytes_to_stream;
    KSLOG_DEBUG("Streamed %d bytes into array", bytes_to_stream);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, bytes_in_array - space_in_buffer);
    end_object(process);
    process->array.is_inside_array = false;
    return CBE_DECODE_STATUS_OK;
}


// ===
// API
// ===

struct cbe_decode_process* cbe_decode_begin(const cbe_decode_callbacks* const callbacks,
                                            void* const user_context)
{
    KSLOG_DEBUG(NULL);
    cbe_decode_process* process = malloc(sizeof(*process));
    memset(process, 0, sizeof(*process));
    process->callbacks = callbacks;
    process->user_context = user_context;
    return process;
}

void* cbe_decode_get_user_context(cbe_decode_process* const process)
{
    return process->user_context;
}

cbe_decode_status cbe_decode_feed(cbe_decode_process* const process,
                                  const uint8_t* const data_start,
                                  const int64_t byte_count)
{
    KSLOG_DEBUG("Feed %d bytes...", byte_count);
    KSLOG_DATA_TRACE(data_start, byte_count, NULL);
    process->buffer.start = data_start;
    process->buffer.position = data_start;
    process->buffer.end = data_start + byte_count;

    #define BEGIN_OBJECT(SIZE) \
        STOP_AND_EXIT_IF_DECODE_STATUS_NOT_OK(begin_object(process, SIZE));
    #define BEGIN_NONKEYABLE_OBJECT(SIZE) \
        STOP_AND_EXIT_IF_DECODE_STATUS_NOT_OK(begin_nonkeyable_object(process, SIZE));
    #define END_OBJECT() \
        end_object(process)

    if(process->array.is_inside_array)
    {
        STOP_AND_EXIT_IF_DECODE_STATUS_NOT_OK(stream_array(process));
    }

    while(process->buffer.position < process->buffer.end)
    {
        KSLOG_DEBUG("Reading type");
        cbe_type_field type = read_uint_8(process);

        switch(type)
        {
            case TYPE_PADDING:
                KSLOG_DEBUG("(Padding)");
                // Ignore and restart loop because padding doesn't count as document content.
                // Otherwise the document depth test would exit the decode loop.
                continue;
            case TYPE_EMPTY:
                KSLOG_DEBUG("(Empty)");
                BEGIN_NONKEYABLE_OBJECT(0);
                STOP_AND_EXIT_IF_FAILED_CALLBACK(process->callbacks->on_add_empty(process));
                END_OBJECT();
                break;
            case TYPE_FALSE:
                KSLOG_DEBUG("(False)");
                BEGIN_OBJECT(0);
                STOP_AND_EXIT_IF_FAILED_CALLBACK(process->callbacks->on_add_boolean(process, false));
                END_OBJECT();
                break;
            case TYPE_TRUE:
                KSLOG_DEBUG("(True)");
                BEGIN_OBJECT(0);
                STOP_AND_EXIT_IF_FAILED_CALLBACK(process->callbacks->on_add_boolean(process, true));
                END_OBJECT();
                break;
            case TYPE_LIST:
                KSLOG_DEBUG("(List)");
                BEGIN_NONKEYABLE_OBJECT(0);
                STOP_AND_EXIT_IF_FAILED_CALLBACK(process->callbacks->on_begin_list(process));
                process->container.level++;
                if(process->container.level < MAX_CONTAINER_DEPTH)
                {
                    process->container.is_inside_map[process->container.level] = false;
                }
                process->container.next_object_is_map_key = false;
                break;
            case TYPE_MAP:
                KSLOG_DEBUG("(Map)");
                BEGIN_NONKEYABLE_OBJECT(0);
                STOP_AND_EXIT_IF_FAILED_CALLBACK(process->callbacks->on_begin_map(process));
                process->container.level++;
                if(process->container.level < MAX_CONTAINER_DEPTH)
                {
                    process->container.is_inside_map[process->container.level] = true;
                }
                process->container.next_object_is_map_key = true;
                break;
            case TYPE_END_CONTAINER:
                KSLOG_DEBUG("(End Container)");
                STOP_AND_EXIT_IF_MAP_VALUE_MISSING(process);
                STOP_AND_EXIT_IF_FAILED_CALLBACK(process->callbacks->on_end_container(process));
                END_OBJECT();
                process->container.level--;
                if(process->container.level < MAX_CONTAINER_DEPTH)
                {
                    process->container.next_object_is_map_key = process->container.is_inside_map[process->container.level];
                }
                break;

            case TYPE_STRING_0: case TYPE_STRING_1: case TYPE_STRING_2: case TYPE_STRING_3:
            case TYPE_STRING_4: case TYPE_STRING_5: case TYPE_STRING_6: case TYPE_STRING_7:
            case TYPE_STRING_8: case TYPE_STRING_9: case TYPE_STRING_10: case TYPE_STRING_11:
            case TYPE_STRING_12: case TYPE_STRING_13: case TYPE_STRING_14: case TYPE_STRING_15:
            {
                int64_t array_byte_count = (int64_t)(type - TYPE_STRING_0);
                KSLOG_DEBUG("(String %d)", array_byte_count);
                STOP_AND_EXIT_IF_FAILED_CALLBACK(process->callbacks->on_begin_string(process, array_byte_count));
                internal_begin_array(process, array_byte_count);
                STOP_AND_EXIT_IF_DECODE_STATUS_NOT_OK(stream_array(process));
                break;
            }

            #define HANDLE_CASE_SCALAR(TYPE, NAME_FRAGMENT) \
                KSLOG_DEBUG("(" #TYPE ")"); \
                BEGIN_OBJECT(sizeof(TYPE)); \
                STOP_AND_EXIT_IF_FAILED_CALLBACK(process->callbacks->on_add_ ## NAME_FRAGMENT(process, read_ ## NAME_FRAGMENT(process))); \
                END_OBJECT();
            case TYPE_INT_16:
                HANDLE_CASE_SCALAR(int16_t, int_16);
                break;
            case TYPE_INT_32:
                HANDLE_CASE_SCALAR(int32_t, int_32);
                break;
            case TYPE_INT_64:
                HANDLE_CASE_SCALAR(int64_t, int_64);
                break;
            case TYPE_INT_128:
                HANDLE_CASE_SCALAR(__int128, int_128);
                break;
            case TYPE_FLOAT_32:
                HANDLE_CASE_SCALAR(float, float_32);
                break;
            case TYPE_FLOAT_64:
                HANDLE_CASE_SCALAR(double, float_64);
                break;
            case TYPE_FLOAT_128:
                HANDLE_CASE_SCALAR(__float128, float_128);
                break;
            case TYPE_DECIMAL_32:
                HANDLE_CASE_SCALAR(_Decimal32, decimal_32);
                break;
            case TYPE_DECIMAL_64:
                HANDLE_CASE_SCALAR(_Decimal64, decimal_64);
                break;
            case TYPE_DECIMAL_128:
                HANDLE_CASE_SCALAR(_Decimal128, decimal_128);
                break;
            case TYPE_TIME:
                HANDLE_CASE_SCALAR(smalltime, time);
                break;
            case TYPE_STRING:
            {
                KSLOG_DEBUG("(String)");
                STOP_AND_EXIT_IF_DECODE_STATUS_NOT_OK(begin_array(process, process->callbacks->on_begin_string));
                STOP_AND_EXIT_IF_DECODE_STATUS_NOT_OK(stream_array(process));
                break;
            }
            case TYPE_BINARY_DATA:
            {
                KSLOG_DEBUG("(Binary)");
                STOP_AND_EXIT_IF_DECODE_STATUS_NOT_OK(begin_array(process, process->callbacks->on_begin_binary));
                STOP_AND_EXIT_IF_DECODE_STATUS_NOT_OK(stream_array(process));
                break;
            }
            default:
                KSLOG_DEBUG("(Small): %d", type);
                BEGIN_OBJECT(0);
                STOP_AND_EXIT_IF_FAILED_CALLBACK(process->callbacks->on_add_int_8(process, type));
                END_OBJECT();
                break;
        }
        if(process->container.level <= 0)
        {
            break;
        }
    }
    return CBE_DECODE_STATUS_OK;
}

int64_t cbe_decode_get_buffer_offset(cbe_decode_process* const process)
{
    return process->buffer.position - process->buffer.start;
}

cbe_decode_status cbe_decode_end(cbe_decode_process* const process)
{
    cbe_decode_process temp_process = *process;
    free(process);
    STOP_AND_EXIT_IF_IS_INSIDE_CONTAINER(&temp_process);
    STOP_AND_EXIT_IF_IS_INSIDE_ARRAY(&temp_process);
    KSLOG_DEBUG("Process ended successfully");
    return CBE_DECODE_STATUS_OK;
}
