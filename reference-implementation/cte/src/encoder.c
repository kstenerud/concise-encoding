#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cte_internal.h"


#define STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(PROCESS, REQUIRED_BYTES) \
    if((size_t)((PROCESS)->end - (PROCESS)->pos) < (size_t)(REQUIRED_BYTES)) \
        return CTE_ENCODE_STATUS_NEED_MORE_ROOM

// TODO: Kil this
static inline cte_encode_process* get_real_process(cte_encode_process* encode_process)
{
    return encode_process;
}

cte_encode_process* cte_encode_begin_with_config(
                        uint8_t* const document_buffer,
                        int64_t byte_count,
                        int indent_spaces,
                        int float_digits_precision)
{
    cte_encode_process* process = malloc(sizeof(*process));
    memset(process, 0, sizeof(*process));
    process->start = document_buffer;
    process->pos = document_buffer;
    process->end = document_buffer + byte_count;
    process->indent_spaces = indent_spaces;
    process->float_digits_precision = float_digits_precision;
    process->container_level = 0;
    process->is_first_in_document = true;
    process->is_first_in_container = false;
    process->next_object_is_map_key = false;
    return process;
}

cte_encode_process* cte_encode_begin(uint8_t* const document_buffer, int64_t byte_count)
{
    return cte_encode_begin_with_config(document_buffer,
                                        byte_count,
                                        DEFAULT_INDENT_SPACES,
                                        DEFAULT_FLOAT_DIGITS_PRECISION);
}

static void add_bytes(cte_encode_process* const process, const char* bytes, size_t length)
{
    memcpy(process->pos, bytes, length);
    process->pos += length;
}

static cte_encode_status add_indentation(cte_encode_process* const process)
{
    if(process->indent_spaces <= 0)
    {
        return CTE_ENCODE_STATUS_OK;
    }

    int num_spaces = process->indent_spaces * process->container_level;
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, num_spaces + 1);
    *process->pos++ = '\n';
    memset(process->pos, ' ', num_spaces);
    process->pos += num_spaces;
    return CTE_ENCODE_STATUS_OK;
}

static cte_encode_status begin_new_object(cte_encode_process* const process)
{
    if(process->is_first_in_document)
    {
        process->is_first_in_document = false;
        return CTE_ENCODE_STATUS_OK;
    }

    bool is_in_map = process->is_inside_map[process->container_level];
    bool next_object_is_map_key = process->next_object_is_map_key;
    cte_encode_status status = CTE_ENCODE_STATUS_OK;

    if(is_in_map)
    {
        process->next_object_is_map_key = !next_object_is_map_key;
    }

    if(process->is_first_in_container)
    {
        if((status = add_indentation(process)) != CTE_ENCODE_STATUS_OK) return status;
        process->is_first_in_container = false;
        return status;
    }

    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, 1);

    if(!is_in_map || next_object_is_map_key)
    {
        *process->pos++ = ',';
        if((status = add_indentation(process)) != CTE_ENCODE_STATUS_OK) return status;
        return status;
    }

    *process->pos++ = ':';
    if(process->indent_spaces > 0)
    {
        STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, 1);
        *process->pos++ = ' ';
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

cte_encode_status cte_encode_add_empty(cte_encode_process* const encode_process)
{
    cte_encode_process* process = get_real_process(encode_process);
    if(process->next_object_is_map_key) return 9999;
    return add_object(process, "empty");
}

cte_encode_status cte_encode_add_boolean(cte_encode_process* const encode_process, const bool value)
{
    cte_encode_process* process = get_real_process(encode_process);
    if(process->next_object_is_map_key) return 9999;
    return add_object(process, value ? "t" : "f");
}

cte_encode_status cte_encode_add_int_64(cte_encode_process* const encode_process, const int64_t value)
{
    cte_encode_process* process = get_real_process(encode_process);
    if(process->next_object_is_map_key) return 9999;
    char buffer[21];
    sprintf(buffer, "%ld", value);
    return add_object(process, buffer);
}

cte_encode_status cte_encode_add_float_64(cte_encode_process* const encode_process, const double value)
{
    cte_encode_process* process = get_real_process(encode_process);
    if(process->next_object_is_map_key) return 9999;
    char fmt[10];
    sprintf(fmt, "%%.%dlg", process->float_digits_precision);
    char buffer[process->float_digits_precision + 2];
    sprintf(buffer, fmt, value);
    return add_object(process, buffer);
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

static cte_encode_status add_substring_with_escaping(cte_encode_process* const process, const char* const start, const int64_t byte_count)
{
    const char* end = start + byte_count;
    for(const char* src = start; src < end; src++)
    {
        char ch = *src;
        char escape_ch = get_escape_char(ch);
        if(escape_ch != 0)
        {
            STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, 2);
            *process->pos++ = '\\';
            *process->pos++ = escape_ch;
        }
        else
        {
            STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, 1);
            *process->pos++ = ch;
        }
    }
    return CTE_ENCODE_STATUS_OK;
}

cte_encode_status cte_encode_add_substring(cte_encode_process* const encode_process, const char* const start, const int64_t byte_count)
{
    cte_encode_process* process = get_real_process(encode_process);
    cte_encode_status status = CTE_ENCODE_STATUS_OK;
    if((status = add_object(process, "\"")) != CTE_ENCODE_STATUS_OK) return status;
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, byte_count + 1);
    add_substring_with_escaping(process, start, byte_count);
    add_bytes(process, "\"", 1);
    return status;
}

cte_encode_status cte_encode_add_string(cte_encode_process* const encode_process, const char* const str)
{
    return cte_encode_add_substring(encode_process, str, strlen(str));
}

static cte_encode_status start_container(cte_encode_process* const process, bool is_map)
{
    if(process->next_object_is_map_key) return 9999;
    begin_new_object(process);
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, 1);
    add_bytes(process, is_map ? "{" : "[", 1);
    process->container_level++;
    process->is_first_in_container = true;
    process->is_inside_map[process->container_level] = is_map;
    process->next_object_is_map_key = is_map;
    return CTE_ENCODE_STATUS_OK;

}

cte_encode_status cte_encode_begin_list(cte_encode_process* const encode_process)
{
    cte_encode_process* process = get_real_process(encode_process);
    return start_container(process, false);
}

cte_encode_status cte_encode_begin_map(cte_encode_process* const encode_process)
{
    cte_encode_process* process = get_real_process(encode_process);
    return start_container(process, true);
}

cte_encode_status cte_encode_end_container(cte_encode_process* const encode_process)
{
    cte_encode_process* process = get_real_process(encode_process);
    cte_encode_status status = CTE_ENCODE_STATUS_OK;
    if(process->container_level <= 0)
    {
        return 9999;
    }
    bool is_in_map = process->is_inside_map[process->container_level];
    if(is_in_map && !process->next_object_is_map_key)
    {
        return 9999;
    }

    process->container_level--;
    if((status = add_indentation(process)) != CTE_ENCODE_STATUS_OK) return status;
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, 1);
    add_bytes(process, is_in_map ? "}" : "]", 1);
    process->next_object_is_map_key = process->is_inside_map[process->container_level];
    return status;
}

cte_encode_status cte_encode_end(cte_encode_process* const encode_process)
{
    cte_encode_process* process = get_real_process(encode_process);
    while(process->container_level > 0)
    {
        if(!cte_encode_end_container(encode_process))
        {
            return 9999;
        }
    }
    STOP_AND_EXIT_IF_NOT_ENOUGH_ROOM(process, 1);
    *process->pos = 0;
    // return (const char*)process->pos;
    return CTE_ENCODE_STATUS_OK;
}
