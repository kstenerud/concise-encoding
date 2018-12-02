#include <memory.h>
#include <stdlib.h>
#include "cbe_internal.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"

typedef struct
{
    cbe_decode_callbacks* callbacks;
    void* user_context;
    const uint8_t* buffer_start;
    const uint8_t* buffer_end;
    const uint8_t* buffer_position;
    const uint8_t* current_object;
    int container_level;
    bool next_object_is_map_key;
    bool is_inside_map[MAX_CONTAINER_DEPTH];
} cbe_real_decode_process;

#define STOP_AND_EXIT_IF_MAP_VALUE_MISSING(PROCESS) \
    if(process->container_level < MAX_CONTAINER_DEPTH) \
        if((PROCESS)->is_inside_map[(PROCESS)->container_level] && !(PROCESS)->next_object_is_map_key) \
            return CBE_DECODE_STATUS_MISSING_VALUE_FOR_KEY

#define STOP_AND_EXIT_IF_EXPECTING_MAP_KEY(PROCESS) \
    if(process->container_level < MAX_CONTAINER_DEPTH) \
    if((PROCESS)->is_inside_map[process->container_level] && (PROCESS)->next_object_is_map_key) \
        return CBE_DECODE_STATUS_INCORRECT_KEY_TYPE

#define STOP_AND_EXIT_IF_NOT_ENOUGH_BYTES(TYPE, BYTE_COUNT) \
    if(process->buffer_position + (BYTE_COUNT) > process->buffer_end) return CBE_DECODE_STATUS_NEED_MORE_DATA

#define STOP_AND_EXIT_IF_BAD_CALLBACK(...) \
    if(!__VA_ARGS__) return CBE_DECODE_STATUS_STOPPED_IN_CALLBACK

static inline void swap_map_key_value_status(cbe_real_decode_process* process)
{
    process->next_object_is_map_key = !process->next_object_is_map_key;
}

static inline void advance_buffer(cbe_real_decode_process* const process, int64_t byte_count)
{
    process->buffer_position += byte_count;
}

static inline uint8_t peek_uint_8(const cbe_real_decode_process* const process)
{
    return *process->buffer_position;
}

static inline uint8_t read_uint_8(cbe_real_decode_process* const process)
{
    return *process->buffer_position++;
}

#define DEFINE_READ_FUNCTION(TYPE, TYPE_SUFFIX) \
static inline TYPE read_ ## TYPE_SUFFIX(cbe_real_decode_process* const process) \
{ \
    const safe_ ## TYPE_SUFFIX* const safe = (safe_ ## TYPE_SUFFIX*)process->buffer_position; \
    advance_buffer(process, sizeof(*safe)); \
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

static inline int peek_array_length_field_width(const cbe_real_decode_process* const process)
{
    switch(peek_uint_8(process) & 3)
    {
        case LENGTH_FIELD_WIDTH_6_BIT:
            return 1;
        case LENGTH_FIELD_WIDTH_14_BIT:
            return 2;
        case LENGTH_FIELD_WIDTH_30_BIT:
            return 4;
        default:
            return 8;
    }
}

static inline int64_t read_array_length(cbe_real_decode_process* const process)
{
    switch(peek_uint_8(process) & 3)
    {
        case LENGTH_FIELD_WIDTH_6_BIT:
            return (int64_t)(read_uint_8(process) >> 2);
        case LENGTH_FIELD_WIDTH_14_BIT:
            return (int64_t)(read_uint_16(process) >> 2);
        case LENGTH_FIELD_WIDTH_30_BIT:
            return (int64_t)(read_uint_32(process) >> 2);
        default:
            return (int64_t)(read_uint_64(process) >> 2);
    }
}

cbe_decode_process* cbe_decode_begin(cbe_decode_callbacks* callbacks, void* user_context)
{
    KSLOG_DEBUG("");
    cbe_real_decode_process* process = malloc(sizeof(*process));
    memset(process, 0, sizeof(*process));
    process->callbacks = callbacks;
    process->user_context = user_context;
    return (cbe_decode_process*)process;
}

void* cbe_decode_get_user_context(cbe_decode_process* decode_process)
{
    cbe_real_decode_process* process = (cbe_real_decode_process*)decode_process;
    return process->user_context;
}

cbe_decode_status cbe_decode_feed(cbe_decode_process* decode_process, const uint8_t* const data_start, const int64_t byte_count)
{
    KSLOG_DEBUG("%d", byte_count);
    KSLOG_DATA_TRACE(data_start, byte_count, "");
    cbe_real_decode_process* process = (cbe_real_decode_process*)decode_process;
    process->buffer_start = data_start;
    process->buffer_position = data_start;
    process->buffer_end = data_start + byte_count;
    process->current_object = process->buffer_position;

    while(process->buffer_position < process->buffer_end)
    {
        cbe_type_field type = read_uint_8(process);
        KSLOG_DEBUG("Type %02x", type);
        switch(type)
        {
            case TYPE_PADDING:
                KSLOG_DEBUG("[Padding]");
                // Ignore and restart loop because padding doesn't count as document content.
                // Otherwise the document depth test would exit the decode loop.
                continue;
            case TYPE_EMPTY:
                KSLOG_DEBUG("[Empty]");
                STOP_AND_EXIT_IF_EXPECTING_MAP_KEY(process);
                STOP_AND_EXIT_IF_BAD_CALLBACK(process->callbacks->on_empty(decode_process));
                swap_map_key_value_status(process);
                break;
            case TYPE_FALSE:
                KSLOG_DEBUG("[False]");
                STOP_AND_EXIT_IF_BAD_CALLBACK(process->callbacks->on_boolean(decode_process, false));
                swap_map_key_value_status(process);
                break;
            case TYPE_TRUE:
                KSLOG_DEBUG("[True]");
                STOP_AND_EXIT_IF_BAD_CALLBACK(process->callbacks->on_boolean(decode_process, true));
                swap_map_key_value_status(process);
                break;
            case TYPE_END_CONTAINER:
                KSLOG_DEBUG("[End Container]");
                STOP_AND_EXIT_IF_MAP_VALUE_MISSING(process);
                STOP_AND_EXIT_IF_BAD_CALLBACK(process->callbacks->on_end_container(decode_process));
                process->container_level--;
                if(process->container_level < MAX_CONTAINER_DEPTH)
                {
                    process->next_object_is_map_key = process->is_inside_map[process->container_level];
                }
                break;
            case TYPE_LIST:
                KSLOG_DEBUG("[List]");
                STOP_AND_EXIT_IF_EXPECTING_MAP_KEY(process);
                STOP_AND_EXIT_IF_BAD_CALLBACK(process->callbacks->on_begin_list(decode_process));
                process->container_level++;
                if(process->container_level < MAX_CONTAINER_DEPTH)
                {
                    process->is_inside_map[process->container_level] = false;
                }
                process->next_object_is_map_key = false;
                break;
            case TYPE_MAP:
                KSLOG_DEBUG("[Map]");
                STOP_AND_EXIT_IF_EXPECTING_MAP_KEY(process);
                STOP_AND_EXIT_IF_BAD_CALLBACK(process->callbacks->on_begin_map(decode_process));
                process->container_level++;
                if(process->container_level < MAX_CONTAINER_DEPTH)
                {
                    process->is_inside_map[process->container_level] = true;
                }
                process->next_object_is_map_key = true;
                break;

            case TYPE_STRING_0: case TYPE_STRING_1: case TYPE_STRING_2: case TYPE_STRING_3:
            case TYPE_STRING_4: case TYPE_STRING_5: case TYPE_STRING_6: case TYPE_STRING_7:
            case TYPE_STRING_8: case TYPE_STRING_9: case TYPE_STRING_10: case TYPE_STRING_11:
            case TYPE_STRING_12: case TYPE_STRING_13: case TYPE_STRING_14: case TYPE_STRING_15:
            {
                int64_t local_byte_count = (int64_t)(type - TYPE_STRING_0);
                KSLOG_DATA_DEBUG(process->buffer_position, local_byte_count, "[String]: ");
                STOP_AND_EXIT_IF_NOT_ENOUGH_BYTES("string", local_byte_count);
                STOP_AND_EXIT_IF_BAD_CALLBACK(process->callbacks->on_string(decode_process, (char*)process->buffer_position, local_byte_count));
                swap_map_key_value_status(process);
                advance_buffer(process, local_byte_count);
                break;
            }

            #define HANDLE_CASE_SCALAR(TYPE, NAME_FRAGMENT) \
            { \
                KSLOG_DEBUG("[" #TYPE "]"); \
                STOP_AND_EXIT_IF_NOT_ENOUGH_BYTES(#NAME_FRAGMENT, sizeof(TYPE)); \
                STOP_AND_EXIT_IF_BAD_CALLBACK(process->callbacks->on_ ## NAME_FRAGMENT(decode_process, read_ ## NAME_FRAGMENT(process))); \
                swap_map_key_value_status(process); \
            }
            case TYPE_INT_16:
                HANDLE_CASE_SCALAR(int16_t, int_16)
                break;
            case TYPE_INT_32:
                HANDLE_CASE_SCALAR(int32_t, int_32)
                break;
            case TYPE_INT_64:
                HANDLE_CASE_SCALAR(int64_t, int_64)
                break;
            case TYPE_INT_128:
                HANDLE_CASE_SCALAR(__int128, int_128)
                break;
            case TYPE_FLOAT_32:
                HANDLE_CASE_SCALAR(float, float_32)
                break;
            case TYPE_FLOAT_64:
                HANDLE_CASE_SCALAR(double, float_64)
                break;
            case TYPE_FLOAT_128:
                HANDLE_CASE_SCALAR(__float128, float_128)
                break;
            case TYPE_DECIMAL_32:
                HANDLE_CASE_SCALAR(_Decimal32, decimal_32)
                break;
            case TYPE_DECIMAL_64:
                HANDLE_CASE_SCALAR(_Decimal64, decimal_64)
                break;
            case TYPE_DECIMAL_128:
                HANDLE_CASE_SCALAR(_Decimal128, decimal_128)
                break;
            case TYPE_TIME:
                HANDLE_CASE_SCALAR(smalltime, time)
                break;
            case TYPE_STRING:
            {
                STOP_AND_EXIT_IF_NOT_ENOUGH_BYTES("string", peek_array_length_field_width(process));
                int64_t local_byte_count = read_array_length(process);
                KSLOG_DATA_DEBUG(process->buffer_position, local_byte_count, "[String]: ");
                STOP_AND_EXIT_IF_NOT_ENOUGH_BYTES("string", local_byte_count);
                STOP_AND_EXIT_IF_BAD_CALLBACK(process->callbacks->on_string(decode_process, (char*)process->buffer_position, local_byte_count));
                swap_map_key_value_status(process);
                advance_buffer(process, local_byte_count);
                break;
            }

            case TYPE_BINARY_DATA:
            {
                STOP_AND_EXIT_IF_NOT_ENOUGH_BYTES("binary data", peek_array_length_field_width(process));
                int64_t local_byte_count = read_array_length(process);
                KSLOG_DATA_DEBUG(process->buffer_position, local_byte_count, "[Binary]: ");
                STOP_AND_EXIT_IF_NOT_ENOUGH_BYTES("binary data", local_byte_count);
                STOP_AND_EXIT_IF_BAD_CALLBACK(process->callbacks->on_binary_data(decode_process, (void*)process->buffer_position, local_byte_count));
                swap_map_key_value_status(process);
                advance_buffer(process, local_byte_count);
                break;
            }
            default:
                KSLOG_DEBUG("[Small]: %d", type);
                STOP_AND_EXIT_IF_BAD_CALLBACK(process->callbacks->on_int_8(decode_process, type));
                swap_map_key_value_status(process);
                break;
        }
        process->current_object = process->buffer_position;
        if(process->container_level <= 0)
        {
            break;
        }
    }
    return CBE_DECODE_STATUS_OK;
}

int64_t cbe_decode_get_buffer_offset(cbe_decode_process* decode_process)
{
    cbe_real_decode_process* process = (cbe_real_decode_process*)decode_process;
    return process->current_object - process->buffer_start;
}

cbe_decode_status cbe_decode_end(cbe_decode_process* decode_process)
{
    cbe_real_decode_process* process = (cbe_real_decode_process*)decode_process;
    int container_level = process->container_level;    
    free(decode_process);
    if(container_level != 0)
    {
        return CBE_DECODE_STATUS_UNBALANCED_CONTAINERS;
    }
    return CBE_DECODE_STATUS_OK;
}
