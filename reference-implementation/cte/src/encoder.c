#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cte/cte.h>

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"


// ====
// Data
// ====

struct cte_encode_process
{
    struct
    {
        const uint8_t* start;
        const uint8_t* end;
        uint8_t* position;
    } buffer;

    int indent_spaces;
    int float_digits_precision;
    bool is_first_in_document;

    struct
    {
        int max_depth;
        int level;
        bool is_first_in_container;
        bool next_object_is_map_key;
        bool is_inside_map[];
    } container;
};
typedef struct cte_encode_process cte_encode_process;


// ==============
// Error Handlers
// ==============

#define STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(PROCESS, REQUIRED_BYTES) \
    if((size_t)((PROCESS)->buffer.end - (PROCESS)->buffer.position) < (size_t)(REQUIRED_BYTES)) \
        return CTE_ENCODE_STATUS_NEED_MORE_ROOM


static inline int get_max_depth_or_default(int max_depth)
{
    return max_depth > 0 ? max_depth : CTE_DEFAULT_MAX_CONTAINER_DEPTH;
}

static inline int get_indent_spaces_or_default(int indent_spaces)
{
    return indent_spaces >= 0 ? indent_spaces : CTE_DEFAULT_INDENT_SPACES;
}

static inline int get_float_digits_precision_or_default(int float_digits_precision)
{
    return float_digits_precision >= 0 ? float_digits_precision : CTE_DEFAULT_FLOAT_DIGITS_PRECISION;
}

static inline void zero_memory(void* const memory, const int byte_count)
{
    uint8_t* ptr = memory;
    uint8_t* const end = ptr + byte_count;
    while(ptr < end)
    {
        *ptr++ = 0;
    }
}


int cte_encode_process_size(const int max_depth)
{
    KSLOG_DEBUG("(max_depth %d", max_depth);
    return sizeof(cte_encode_process) + max_depth;
}

cte_encode_status cte_encode_begin(cte_encode_process* process,
                                   uint8_t* const document_buffer,
                                   int64_t byte_count,
                                   int max_depth,
                                   int float_digits_precision,
                                   int indent_spaces)
{
    KSLOG_DEBUG("(process %p, document_buffer %p, byte_count %d, max_depth %d, float_digits_precision %d, indent_spaces %d)",
        process, document_buffer, byte_count, max_depth, float_digits_precision, indent_spaces);
    zero_memory(process, sizeof(process) + 1);
    process->buffer.start = document_buffer;
    process->buffer.position = document_buffer;
    process->buffer.end = document_buffer + byte_count;
    process->indent_spaces = get_indent_spaces_or_default(indent_spaces);
    process->float_digits_precision = get_float_digits_precision_or_default(float_digits_precision);
    process->container.max_depth = get_max_depth_or_default(max_depth);
    process->is_first_in_document = true;
    return CTE_ENCODE_STATUS_OK;
}

static void add_bytes(cte_encode_process* const process, const char* bytes, size_t length)
{
    memcpy(process->buffer.position, bytes, length);
    process->buffer.position += length;
}

static cte_encode_status add_indentation(cte_encode_process* const process)
{
    if(process->indent_spaces <= 0)
    {
        return CTE_ENCODE_STATUS_OK;
    }

    int num_spaces = process->indent_spaces * process->container.level;
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, num_spaces + 1);
    *process->buffer.position++ = '\n';
    memset(process->buffer.position, ' ', num_spaces);
    process->buffer.position += num_spaces;
    return CTE_ENCODE_STATUS_OK;
}

static cte_encode_status begin_new_object(cte_encode_process* const process)
{
    if(process->is_first_in_document)
    {
        process->is_first_in_document = false;
        return CTE_ENCODE_STATUS_OK;
    }

    bool is_in_map = process->container.is_inside_map[process->container.level];
    bool next_object_is_map_key = process->container.next_object_is_map_key;
    cte_encode_status status = CTE_ENCODE_STATUS_OK;

    if(is_in_map)
    {
        process->container.next_object_is_map_key = !next_object_is_map_key;
    }

    if(process->container.is_first_in_container)
    {
        if((status = add_indentation(process)) != CTE_ENCODE_STATUS_OK) return status;
        process->container.is_first_in_container = false;
        return status;
    }

    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, 1);

    if(!is_in_map || next_object_is_map_key)
    {
        *process->buffer.position++ = ',';
        if((status = add_indentation(process)) != CTE_ENCODE_STATUS_OK) return status;
        return status;
    }

    *process->buffer.position++ = ':';
    if(process->indent_spaces > 0)
    {
        STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, 1);
        *process->buffer.position++ = ' ';
    }
    return status;
}

static cte_encode_status add_object(cte_encode_process* const process, const char* encoded_object)
{
    cte_encode_status status = CTE_ENCODE_STATUS_OK;
    if((status = begin_new_object(process)) != CTE_ENCODE_STATUS_OK) return status;
    int length = strlen(encoded_object);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, length);
    add_bytes(process, encoded_object, length);
    return status;
}

cte_encode_status cte_encode_add_nil(cte_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    if(process->container.next_object_is_map_key) return 9999;
    return add_object(process, "nil");
}

cte_encode_status cte_encode_add_boolean(cte_encode_process* const process, const bool value)
{
    KSLOG_DEBUG("(process %p, value %d)", process, value);
    if(process->container.next_object_is_map_key) return 9999;
    return add_object(process, value ? "true" : "false");
}

cte_encode_status cte_encode_add_int_8(cte_encode_process* const process, const int8_t value)
{
    //TODO
    return cte_encode_add_int_64(process, value);
}

cte_encode_status cte_encode_add_int_16(cte_encode_process* const process, const int16_t value)
{
    //TODO
    return cte_encode_add_int_64(process, value);
}

cte_encode_status cte_encode_add_int_32(cte_encode_process* const process, const int32_t value)
{
    //TODO
    return cte_encode_add_int_64(process, value);
}

cte_encode_status cte_encode_add_int_64(cte_encode_process* const process, const int64_t value)
{
    KSLOG_DEBUG("(process %p, value %d)", process, value);
    if(process->container.next_object_is_map_key) return 9999;
    char buffer[21];
    sprintf(buffer, "%ld", value);
    return add_object(process, buffer);
}

cte_encode_status cte_encode_add_int_128(cte_encode_process* const process, const __int128_t value)
{
    // TODO
    return cte_encode_add_int_64(process, (int64_t)value);
}

cte_encode_status cte_encode_add_float_32(cte_encode_process* const process, const float value)
{
    //TODO
    return cte_encode_add_float_64(process, value);
}

cte_encode_status cte_encode_add_float_64(cte_encode_process* const process, const double value)
{
    KSLOG_DEBUG("(process %p, value %f)", process, value);
    if(process->container.next_object_is_map_key) return 9999;
    char fmt[10];
    sprintf(fmt, "%%.%dlg", process->float_digits_precision);
    char buffer[process->float_digits_precision + 2];
    sprintf(buffer, fmt, value);
    return add_object(process, buffer);
}

cte_encode_status cte_encode_add_float_128(cte_encode_process* const process, const __float128 value)
{
    //TODO
    return cte_encode_add_float_64(process, (double)value);
}

cte_encode_status cte_encode_add_decimal_32(cte_encode_process* const process, const _Decimal32 value)
{
    // TODO
    (void)process;
    (void)value;
    return CTE_DECODE_ERROR_INTERNAL_BUG;
}

cte_encode_status cte_encode_add_decimal_64(cte_encode_process* const process, const _Decimal64 value)
{
    // TODO
    (void)process;
    (void)value;
    return CTE_DECODE_ERROR_INTERNAL_BUG;
}

cte_encode_status cte_encode_add_decimal_128(cte_encode_process* const process, const _Decimal128 value)
{
    // TODO
    (void)process;
    (void)value;
    return CTE_DECODE_ERROR_INTERNAL_BUG;
}

cte_encode_status cte_encode_add_time(cte_encode_process* const process, const smalltime value)
{
    // TODO
    (void)process;
    (void)value;
    return CTE_DECODE_ERROR_INTERNAL_BUG;
}

static char get_escape_char(char ch)
{
    switch(ch)
    {
        case '\\': return '\\';
        case  '"': return '"';
        case '\b': return 'b';
        case '\f': return 'f';
        case '\n': return 'n';
        case '\r': return 'r';
        case '\t': return 't';
        default:   return 0;
    }
}

static cte_encode_status add_string_with_escaping(cte_encode_process* const process, const char* const start, const int64_t byte_count)
{
    const char* end = start + byte_count;
    for(const char* src = start; src < end; src++)
    {
        char ch = *src;
        char escape_ch = get_escape_char(ch);
        if(escape_ch != 0)
        {
            STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, 2);
            *process->buffer.position++ = '\\';
            *process->buffer.position++ = escape_ch;
        }
        else
        {
            STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, 1);
            *process->buffer.position++ = ch;
        }
    }
    return CTE_ENCODE_STATUS_OK;
}

cte_encode_status cte_encode_add_string(cte_encode_process* const process, const char* const start, const int64_t byte_count)
{
    KSLOG_DEBUG("(process %p, start %p, byte_count %d)", process, start, byte_count);
    cte_encode_status status = CTE_ENCODE_STATUS_OK;
    if((status = add_object(process, "\"")) != CTE_ENCODE_STATUS_OK) return status;
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, byte_count + 1);
    add_string_with_escaping(process, start, byte_count);
    add_bytes(process, "\"", 1);
    return status;
}

static cte_encode_status start_container(cte_encode_process* const process, bool is_map)
{
    if(process->container.next_object_is_map_key) return 9999;
    begin_new_object(process);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, 1);
    add_bytes(process, is_map ? "{" : "[", 1);
    process->container.level++;
    process->container.is_first_in_container = true;
    process->container.is_inside_map[process->container.level] = is_map;
    process->container.next_object_is_map_key = is_map;
    return CTE_ENCODE_STATUS_OK;

}

cte_encode_status cte_encode_list_begin(cte_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    return start_container(process, false);
}

cte_encode_status cte_encode_map_begin(cte_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    return start_container(process, true);
}

cte_encode_status cte_encode_container_end(cte_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    cte_encode_status status = CTE_ENCODE_STATUS_OK;
    if(process->container.level <= 0)
    {
        return 9999;
    }
    bool is_in_map = process->container.is_inside_map[process->container.level];
    if(is_in_map && !process->container.next_object_is_map_key)
    {
        return 9999;
    }

    process->container.level--;
    if((status = add_indentation(process)) != CTE_ENCODE_STATUS_OK) return status;
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, 1);
    add_bytes(process, is_in_map ? "}" : "]", 1);
    process->container.next_object_is_map_key = process->container.is_inside_map[process->container.level];
    return status;
}

cte_encode_status cte_encode_end(cte_encode_process* const process)
{
    KSLOG_DEBUG("(process %p)", process);
    while(process->container.level > 0)
    {
        if(!cte_encode_container_end(process))
        {
            return 9999;
        }
    }
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, 1);
    *process->buffer.position = 0;
    // return (const char*)process->buffer.position;
    return CTE_ENCODE_STATUS_OK;
}
