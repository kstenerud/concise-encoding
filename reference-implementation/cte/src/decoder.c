#include <errno.h>
#include <limits.h>
#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include "decoder.h"


static bool is_hex_character(int ch)
{
    if(ch >= '0' && ch <= '9')
    {
        return true;
    }
    return ch >= 'a' && ch <= 'f';
}

/**
 * Unquotes and unescapes a string in-place (modifies the original string)
 *
 * @param str The string to unescape (will be modified).
 * @return NULL if successful, or else a pointer to the offending escape sequence.
 */
static char* string_unquote_unescape(char* str)
{
    char* read_pos = str;
    char* read_end = read_pos + strlen(read_pos);
    char* write_pos = str;

    if(*read_pos != '"')
    {
        return read_pos;
    }
    read_pos++;

    for(; read_pos < read_end; read_pos++)
    {
        switch(*read_pos)
        {
            case '"':
                *write_pos = 0;
                return NULL;
            case '\\':
            {
                char* escape_pos = read_pos;
                int remaining_bytes = read_end - read_pos;
                if(remaining_bytes < 1)
                {
                    return escape_pos;
                }

                read_pos++;
                switch(*read_pos)
                {
                    case 'r': *write_pos++ = '\r'; break;
                    case 'n': *write_pos++ = '\n'; break;
                    case 't': *write_pos++ = '\t'; break;
                    case '\\': *write_pos++ = '\\'; break;
                    case '\"': *write_pos++ = '\"'; break;
                    case 'x':
                    {
                        if(remaining_bytes < 2)
                        {
                            return escape_pos;
                        }
                        read_pos++;
                        if(!is_hex_character(*read_pos))
                        {
                            return escape_pos;
                        }
                        uint8_t number_buffer[3] = {*read_pos, 0, 0};
                        read_pos += 1;
                        if(is_hex_character(*read_pos))
                        {
                            number_buffer[1] = *read_pos;
                            read_pos += 1;
                        }
                        *write_pos++ = (uint8_t)strtoul((char*)number_buffer, NULL, 16);
                        read_pos--;
                        break;
                    }
                    case 'u':
                    {
                        if(remaining_bytes < 5)
                        {
                            return escape_pos;
                        }
                        if(!is_hex_character(read_pos[1])
                        || !is_hex_character(read_pos[2])
                        || !is_hex_character(read_pos[3])
                        || !is_hex_character(read_pos[4]))
                        {
                            return escape_pos;
                        }
                        uint8_t number_buffer[5] = {read_pos[1], read_pos[2], read_pos[3], read_pos[4], 0};
                        read_pos += 4;
                        unsigned int codepoint = strtoul((char*)number_buffer, NULL, 16);
                        if(codepoint <= 0x7f)
                        {
                            *write_pos++ = (uint8_t)codepoint;
                        }
                        else if(codepoint <= 0x7ff)
                        {
                            *write_pos++ = (uint8_t)((codepoint >> 6) | 0xc0);
                            *write_pos++ = (uint8_t)((codepoint & 0x3f) | 0x80);
                        }
                        else
                        {
                            *write_pos++ = (uint8_t)((codepoint >> 12) | 0xe0);
                            *write_pos++ = (uint8_t)(((codepoint >> 6) & 0x3f) | 0x80);
                            *write_pos++ = (uint8_t)((codepoint & 0x3f) | 0x80);
                        }
                        break;
                    }
                    default:
                        return escape_pos;
                }
                break;
            }
            default:
                *write_pos++ = *read_pos;
                break;
        }
    }
    *write_pos = 0;
    return NULL;
}


static void on_error(cte_decode_process* process, const char* message)
{
	process->callbacks->on_error(process, message);
}

static bool on_nil(cte_decode_process* process)
{
    return process->callbacks->on_nil(process);
}

static bool on_true(cte_decode_process* process)
{
    return process->callbacks->on_boolean(process, true);
}

static bool on_false(cte_decode_process* process)
{
    return process->callbacks->on_boolean(process, false);
}

static bool on_int(cte_decode_process* process, int base, char* string_value)
{
    int64_t value = strtoll(string_value, NULL, base);
    if((value == LLONG_MAX || value == LLONG_MIN) && errno == ERANGE)
    {
    	process->callbacks->on_error(process, "Number out of range");
    	return false;
    }
    return process->callbacks->on_int_64(process, value);
}

static bool on_float(cte_decode_process* process, char* string_value)
{
    double value = strtod(string_value, NULL);
    if((value == HUGE_VAL || value == -HUGE_VAL) && errno == ERANGE)
    {
    	process->callbacks->on_error(process, "Number out of range");
    	return false;
    }
    return process->callbacks->on_float_64(process, value);
}

static bool on_decimal(cte_decode_process* process, char* string_value)
{
	// TODO
	(void)process;
	(void)string_value;
	return true;
}

static bool on_time(cte_decode_process* process, int day_digits, char* string_value)
{
	// TODO
	(void)process;
    (void)day_digits;
	(void)string_value;
	return true;
}

static bool on_list_begin(cte_decode_process* process)
{
    return process->callbacks->on_list_begin(process);
}

static bool on_list_end(cte_decode_process* process)
{
    return process->callbacks->on_list_end(process);
}

static bool on_map_begin(cte_decode_process* process)
{
    return process->callbacks->on_map_begin(process);
}

static bool on_map_end(cte_decode_process* process)
{
    return process->callbacks->on_map_end(process);
}

static bool on_string(cte_decode_process* process, char* value)
{
    char* bad_data_loc = string_unquote_unescape(value);
    if(bad_data_loc != NULL)
    {
    	// TODO: Show where it's bad (at bad_data_log)
    	process->callbacks->on_error(process, "Bad string");
    	return false;
    }
    bool result = process->callbacks->on_string_begin(process);
    if(!result) return false;
    result = process->callbacks->on_string_data(process, value, strlen(value));
    if(!result) return false;
    return process->callbacks->on_string_end(process);
}

static bool on_binary_data_begin(cte_decode_process* process)
{
	// TODO
	(void)process;
	return true;
}

static bool on_binary_data_end(cte_decode_process* process)
{
    return process->callbacks->on_binary_end(process);
}

// Exposed global so that cte.l can see it
internal_parse_callbacks g_cte_parse_callbacks =
{
    .on_error = on_error,
    .on_nil = on_nil,
    .on_true = on_true,
    .on_false = on_false,
    .on_int = on_int,
    .on_float = on_float,
    .on_decimal = on_decimal,
    .on_time = on_time,
    .on_list_begin = on_list_begin,
    .on_list_end = on_list_end,
    .on_map_begin = on_map_begin,
    .on_map_end = on_map_end,
    .on_string = on_string,
    .on_binary_data_begin = on_binary_data_begin,
    .on_binary_data_end = on_binary_data_end,
};

int cte_decode_process_size(int max_container_depth)
{
    (void)max_container_depth;
    return sizeof(cte_decode_process);
}

void* cte_decode_get_user_context(cte_decode_process* process)
{
    return process->user_context;
}
