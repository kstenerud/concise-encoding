#include <cte/cte.h>
#include <math.h>
#include <limits.h>

// #define KSLogger_LocalLevel TRACE
#include "kslogger.h"

#define ARRAY_BUFFER_SIZE 1000


#define FLOAT32_EXP_MIN -126
#define FLOAT32_EXP_MAX  127
#define FLOAT32_SIGNIFICAND_BITS (24-1)
#define FLOAT32_SIGNIFICAND_MAX ((1L<<FLOAT32_SIGNIFICAND_BITS) - 1)
#define FLOAT64_EXP_MIN -1022
#define FLOAT64_EXP_MAX  1023
#define FLOAT64_SIGNIFICAND_BITS (53-1)
#define FLOAT64_SIGNIFICAND_MAX ((1L<<FLOAT64_SIGNIFICAND_BITS) - 1)

// ====
// Data
// ====

typedef enum
{
    ARRAY_TYPE_NONE,
    ARRAY_TYPE_BINARY_HEX,
    ARRAY_TYPE_BINARY_SAFE85,
    ARRAY_TYPE_STRING,
} array_type;

struct cte_decode_process
{
    const cte_decode_callbacks* callbacks;
    void* user_context;
    array_type processing_array_type;
    const char* token_start;
    const char* array_start;
    __int128_t number_significand;
    int number_sign;
    int number_exponent;
    int number_exponent_sign;
    int number_base;
    int number_fp_dot_position;
    int significand_digit_count;
    bool number_is_fp;
    bool number_is_decimal_fp;
    bool number_has_encountered_dot;
    bool number_is_infinity;
    bool number_is_nan;
    int time_year;
    int time_month;
    int time_day;
    int time_hour;
    int time_minute;
    int time_second;
    int time_microsecond;
    bool time_is_doy;

    uint8_t array_buffer[ARRAY_BUFFER_SIZE];
    int array_buffer_offset;

    cte_decode_status state;
    int max_container_depth;

    // Ragel variables
    int cs;
    int top;
    uint16_t stack[];
};
typedef struct cte_decode_process cte_decode_process;

static uint8_t g_hex_values[] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
};

static int g_days_to_the_month[] =
{
    0, // Nothing
    0, // January
    31, // February
    31 + 28, // March
    31 + 28 + 31, // April
    31 + 28 + 31 + 30, // May
    31 + 28 + 31 + 30 + 31, // June
    31 + 28 + 31 + 30 + 31 + 30, // July
    31 + 28 + 31 + 30 + 31 + 30 + 31, // August
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31, // September
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30, // October
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31, // November
    31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30, // December
};

static inline int is_leap_year(int year)
{
    return (year % 4 == 0) && (year % 100 != 0 || year % 400 == 0);
}

static int ymd_to_doy(int year, int month, int day)
{
    int days = g_days_to_the_month[month] + day;
    if(is_leap_year(year) && month > 2)
    {
        days++;
    }
    return days;
}


static _Decimal128 bugfix_cast_int128_to_decimal(__int128_t value)
{
    // TODO: Bug workaround:
    // Cast from int128 to decimal causes undefined reference to `__bid_floattitd'

    // 64 bit works fine.
    if(value >= INT64_MIN && value <= INT64_MAX)
    {
        return (_Decimal128)(int64_t)value;
    }

    _Decimal128 result = (int64_t)(value >> 64);
    result *= ((uint64_t)1 << 63);
    result *= 2;
    result += (uint64_t)(value & 0xffffffffffffffffL);
    return result;
    // This may lose accuracy
    // return (_Decimal128)(__float128)value;
}

static __float128 fpow128(__float128 value, int exponent)
{
    // TODO: Replace this once 128 bit pow is in std library
    for(int i = exponent; i > 0; i--)
    {
        value *= 10;
    }
    for(int i = exponent; i < 0; i++)
    {
        value /= 10;
    }
    return value;
}

static _Decimal128 dpow128(_Decimal128 value, int exponent)
{
    // TODO: Replace this once 128 bit pow is in std library
    for(int i = exponent; i > 0; i--)
    {
        value *= 10;
    }
    for(int i = exponent; i < 0; i++)
    {
        value /= 10;
    }
    return value;
}

static bool output_number(cte_decode_process* process)
{
    KSLOG_DEBUG("(process %p)", process);
    const __int128_t significand = process->number_significand * process->number_sign;
    if(process->number_is_fp)
    {
        if(process->number_is_infinity)
        {
            return process->callbacks->on_float_32(process, INFINITY * process->number_sign);
        }
        if(process->number_is_nan)
        {
            return process->callbacks->on_float_32(process, NAN);
        }

        const int exponent = (process->number_exponent * process->number_exponent_sign) - process->number_fp_dot_position;

        if(process->number_is_decimal_fp)
        {
            _Decimal128 value = dpow128(bugfix_cast_int128_to_decimal(significand), exponent);

            if(value == (_Decimal32)value)
            {
                return process->callbacks->on_decimal_32(process, (_Decimal32)value);
            }
            if(value == (_Decimal64)value)
            {
                return process->callbacks->on_decimal_64(process, (_Decimal64)value);
            }
            return process->callbacks->on_decimal_128(process, value);
        }

        __float128 value = fpow128(significand, exponent);

        if(exponent >= FLOAT32_EXP_MIN && exponent <= FLOAT32_EXP_MAX &&
            significand >= -FLOAT32_SIGNIFICAND_MAX && significand <= FLOAT32_SIGNIFICAND_MAX)
        {
            return process->callbacks->on_float_32(process, (float)value);
        }
        if(exponent >= FLOAT64_EXP_MIN && exponent <= FLOAT64_EXP_MAX &&
            significand >= -FLOAT64_SIGNIFICAND_MAX && significand <= FLOAT64_SIGNIFICAND_MAX)
        {
            return process->callbacks->on_float_64(process, (double)value);
        }
        return process->callbacks->on_float_128(process, value);
    }
    else
    {
        if(significand >= INT_MIN && significand <= INT_MAX)
        {
            if(sizeof(int) == sizeof(__int128))
            {
                return process->callbacks->on_int_128(process, significand);
            }
            if(sizeof(int) == sizeof(int64_t))
            {
                return process->callbacks->on_int_64(process, significand);
            }
            if(sizeof(int) == sizeof(int32_t))
            {
                return process->callbacks->on_int_32(process, significand);
            }
            if(sizeof(int) == sizeof(int16_t))
            {
                return process->callbacks->on_int_16(process, significand);
            }
            return process->callbacks->on_int_8(process, significand);
        }
        if(sizeof(int) == sizeof(int32_t) &&
           significand >= INT64_MIN && significand <= INT64_MAX)
        {
            return process->callbacks->on_int_64(process, significand);
        }
        return process->callbacks->on_int_128(process, significand);
    }
}

static bool output_time(cte_decode_process* process)
{
    if(process->time_is_doy)
    {
        // TODO: Validate
    }
    else
    {
        // TODO: Validate
        process->time_day = ymd_to_doy(process->time_year, process->time_month, process->time_day);
    }
    smalltime time = 0;
    time = smalltime_with_year(time, process->time_year);
    time = smalltime_with_day(time, process->time_day);
    time = smalltime_with_hour(time, process->time_hour);
    time = smalltime_with_minute(time, process->time_minute);
    time = smalltime_with_second(time, process->time_second);
    time = smalltime_with_microsecond(time, process->time_microsecond);
    return process->callbacks->on_time(process, time);
}

static bool output_binary_data(cte_decode_process* process)
{
    // TODO: Decoding
    bool result = process->callbacks->on_binary_data(process,
        process->array_buffer, process->array_buffer_offset);
    process->array_buffer_offset = 0;
    return result;
}

static bool add_binary_character(cte_decode_process* process, char ch)
{
    // TODO: Actually decode the data
    process->array_buffer[process->array_buffer_offset++] = ch;

    if(process->array_buffer_offset >= (int)sizeof(process->array_buffer))
    {
        return output_binary_data(process);
    }
    return true;
}

static bool output_string_data(cte_decode_process* process)
{
    bool result = process->callbacks->on_string_data(process,
        (char*)process->array_buffer, process->array_buffer_offset);
    process->array_buffer_offset = 0;
    return result;
}

static bool add_string_character(cte_decode_process* process, char ch)
{
    process->array_buffer[process->array_buffer_offset++] = ch;

    if(process->array_buffer_offset >= (int)sizeof(process->array_buffer))
    {
        return output_string_data(process);
    }
    return true;
}

static void handle_error(cte_decode_process* process, const char* current_pointer)
{
    KSLOG_DEBUG("(process %p, current_pointer %p %c)", process, current_pointer, *current_pointer);
    char buffer[1000];
    if(process->token_start == NULL)
    {
        snprintf(buffer, sizeof(buffer), "Error parsing data: %s", current_pointer);
        buffer[sizeof(buffer)-1] = 0;
    }
    else
    {
        sprintf(buffer, "Error parsing token: ");
        int used_bytes = strlen(buffer);
        int token_length = current_pointer - process->token_start;
        KSLOG_TRACE("memcpy(%p + %d, %p, %d", buffer,used_bytes, process->token_start, token_length);
        memcpy(buffer+used_bytes, process->token_start, token_length);
        buffer[used_bytes+token_length] = 0;
    }
    // TODO: Use return code instead
    // process->callbacks->on_error(process, buffer);
}

%%{
    machine cte;
    access process->;


    ###########
    # Actions #
    ###########

    prepush
    {
        KSLOG_TRACE("prepush");
        if(process->top >= process->max_container_depth)
        {
            // TODO: Use return code instead
            // process->callbacks->on_error(process, "Document max depth exceeded");
            fbreak;
        }
    }

    action on_nil
    {
        KSLOG_TRACE("on_nil");
        if(!process->callbacks->on_nil(process))
        {
            status = CTE_DECODE_STATUS_STOPPED_IN_CALLBACK;
            fbreak;
        }
        process->token_start = NULL;
    }

    action on_true
    {
        KSLOG_TRACE("on_true");
        if(!process->callbacks->on_boolean(process, true))
        {
            status = CTE_DECODE_STATUS_STOPPED_IN_CALLBACK;
            fbreak;
        }
        process->token_start = NULL;
    }

    action on_false
    {
        KSLOG_TRACE("on_false");
        if(!process->callbacks->on_boolean(process, false))
        {
            status = CTE_DECODE_STATUS_STOPPED_IN_CALLBACK;
            fbreak;
        }
        process->token_start = NULL;
    }

    action on_time_start
    {
        KSLOG_TRACE("on_time_start");
        process->token_start = p;
        process->time_year = 0;
        process->time_month = 0;
        process->time_day = 0;
        process->time_hour = 0;
        process->time_minute = 0;
        process->time_second = 0;
        process->time_microsecond = 0;
        process->time_is_doy = false;
    }

    action on_time_year_digit
    {
        KSLOG_TRACE("on_time_year_digit: [%c]", *p);
        process->time_year = process->time_year * 10 + *p - '0';
    }

    action on_time_month_digit
    {
        KSLOG_TRACE("on_time_month_digit: [%c]", *p);
        process->time_month = process->time_month * 10 + *p - '0';
    }

    action on_time_day_digit
    {
        KSLOG_TRACE("on_time_day_digit: [%c]", *p);
        process->time_day = process->time_day * 10 + *p - '0';
    }

    action on_time_hour_digit
    {
        KSLOG_TRACE("on_time_hour_digit: [%c]", *p);
        process->time_hour = process->time_hour * 10 + *p - '0';
    }

    action on_time_minute_digit
    {
        KSLOG_TRACE("on_time_minute_digit: [%c]", *p);
        process->time_minute = process->time_minute * 10 + *p - '0';
    }

    action on_time_second_digit
    {
        KSLOG_TRACE("on_time_second_digit: [%c]", *p);
        process->time_second = process->time_second * 10 + *p - '0';
    }

    action on_time_microsecond_digit
    {
        KSLOG_TRACE("on_time_microsecond_digit: [%c]", *p);
        process->time_microsecond = process->time_microsecond * 10 + *p - '0';
    }

    action on_time_doy
    {
        KSLOG_TRACE("on_time_doy");
        process->time_is_doy = true;
    }

    action on_time_end
    {
        KSLOG_TRACE("on_time_end");
        if(!output_time(process))
        {
            status = CTE_DECODE_STATUS_STOPPED_IN_CALLBACK;
            fbreak;
        }
        process->token_start = NULL;
    }

    action on_number_begin
    {
        KSLOG_TRACE("on_number_begin");
        process->token_start = p;
        process->number_significand = 0;
        process->number_base = 10;
        process->number_sign = 1;
        process->number_exponent = 0;
        process->number_exponent_sign = 1;
        process->number_fp_dot_position = 0;
        process->number_is_fp = false;
        process->number_is_decimal_fp = false;
        process->number_has_encountered_dot = false;
        process->significand_digit_count = 0;
        process->number_is_infinity = false;
        process->number_is_nan = false;
    }

    action on_number_base_2
    {
        KSLOG_TRACE("on_number_base_2");
        process->number_base = 2;
    }

    action on_number_base_8
    {
        KSLOG_TRACE("on_number_base_8");
        process->number_base = 8;
    }

    action on_number_base_16
    {
        KSLOG_TRACE("on_number_base_16");
        process->number_base = 16;
    }

    action on_number_decimal
    {
        KSLOG_TRACE("on_number_decimal");
        process->number_is_fp = true;
        process->number_is_decimal_fp = true;
    }

    action on_number_negative
    {
        KSLOG_TRACE("on_number_negative");
        process->number_sign = -1;
    }

    action on_number_significand_digit
    {
        KSLOG_TRACE("on_number_significand_digit: [%c]", *p);
        process->number_significand = process->number_significand * process->number_base + g_hex_values[*p - '0'];
        process->significand_digit_count++;
        if(process->number_has_encountered_dot)
        {
            process->number_fp_dot_position++;
        }
    }

    action on_number_dot
    {
        KSLOG_TRACE("on_number_dot");
        process->number_has_encountered_dot = true;
        process->number_is_fp = true;
    }

    action on_number_exponent_negative
    {
        KSLOG_TRACE("on_number_exponent_negative");
        process->number_exponent_sign = -1;
    }

    action on_number_exponent_digit
    {
        KSLOG_TRACE("on_number_exponent_digit: [%c]", *p);
        process->number_exponent = process->number_exponent * 10 + *p - '0';
    }

    action on_number_infinity
    {
        KSLOG_TRACE("on_number_infinity");
        process->number_is_infinity = true;
        process->number_is_fp = true;
    }

    action on_number_nan
    {
        KSLOG_TRACE("on_number_nan");
        process->number_is_nan = true;
        process->number_is_fp = true;
    }

    action on_number_end
    {
        KSLOG_TRACE("on_number_end");
        if(!output_number(process))
        {
            status = CTE_DECODE_STATUS_STOPPED_IN_CALLBACK;
            fbreak;
        }
        process->token_start = NULL;
    }

    action on_binary_type_hex
    {
        KSLOG_TRACE("on_binary_type_hex");
        process->processing_array_type = ARRAY_TYPE_BINARY_HEX;
    }

    action on_binary_type_safe85
    {
        KSLOG_TRACE("on_binary_type_safe85");
        process->processing_array_type = ARRAY_TYPE_BINARY_SAFE85;
    }

    action on_binary_begin
    {
        KSLOG_TRACE("on_binary_begin");
        process->token_start = p;
        process->array_start = p;
        if(!process->callbacks->on_binary_begin(process))
        {
            status = CTE_DECODE_STATUS_STOPPED_IN_CALLBACK;
            fbreak;
        }
    }

    action on_binary_character
    {
        KSLOG_TRACE("on_binary_character: [%c]", *p);
        add_binary_character(process, *p);
    }

    action on_binary_end
    {
        KSLOG_TRACE("on_binary_end");
        if(!output_binary_data(process))
        {
            status = CTE_DECODE_STATUS_STOPPED_IN_CALLBACK;
            fbreak;
        }
        if(!process->callbacks->on_binary_end(process))
        {
            status = CTE_DECODE_STATUS_STOPPED_IN_CALLBACK;
            fbreak;
        }
        process->processing_array_type = ARRAY_TYPE_NONE;
        process->token_start = NULL;
    }

    action on_string_begin
    {
        KSLOG_TRACE("on_string_begin");
        process->token_start = p;
        if(!process->callbacks->on_string_begin(process))
        {
            status = CTE_DECODE_STATUS_STOPPED_IN_CALLBACK;
            fbreak;
        }
    }

    action on_string_character
    {
        KSLOG_TRACE("on_string_character: [%c]", *p);
        add_string_character(process, *p);
    }

    action on_string_character_double_quote
    {
        KSLOG_TRACE("on_string_character_double_quote");
        add_string_character(process, '"');
    }

    action on_string_character_newline
    {
        KSLOG_TRACE("on_string_character_newline");
        add_string_character(process, '\n');
    }

    action on_string_character_carriage_return
    {
        KSLOG_TRACE("on_string_character_carriage_return");
        add_string_character(process, '\r');
    }

    action on_string_character_tab
    {
        KSLOG_TRACE("on_string_character_tab");
        add_string_character(process, '\t');
    }

    action on_string_character_backslash
    {
        KSLOG_TRACE("on_string_character_backslash");
        add_string_character(process, '\\');
    }

    action on_string_character_hex
    {
        KSLOG_TRACE("on_string_character_hex");
        uint8_t value = (g_hex_values[(int)p[-2] - '0'] << 4) | g_hex_values[(int)p[-1] - '0'];
        KSLOG_TRACE("%c, %c: %02x", p[-2], p[-1], value);
        add_string_character(process, value);
    }

    action on_string_character_unicode
    {
        KSLOG_TRACE("on_string_character_unicode");
        int codepoint = (g_hex_values[(int)p[-4] - '0'] << 12) |
                        (g_hex_values[(int)p[-3] - '0'] << 8) |
                        (g_hex_values[(int)p[-2] - '0'] << 4) |
                        g_hex_values[(int)p[-1] - '0'];
        KSLOG_TRACE("%c, %c, %c, %c: %02x", p[-4], p[-3], p[-2], p[-1], codepoint);
        if(codepoint <= 0x7f)
        {
            add_string_character(process, (uint8_t)codepoint);
        }
        else if(codepoint <= 0x7ff)
        {
            add_string_character(process, (uint8_t)((codepoint >> 6) | 0xc0));
            add_string_character(process, (uint8_t)((codepoint & 0x3f) | 0x80));
        }
        else
        {
            add_string_character(process, (uint8_t)((codepoint >> 12) | 0xe0));
            add_string_character(process, (uint8_t)(((codepoint >> 6) & 0x3f) | 0x80));
            add_string_character(process, (uint8_t)((codepoint & 0x3f) | 0x80));
        }
    }

    action on_string_end
    {
        KSLOG_TRACE("on_string_end");
        if(!output_string_data(process))
        {
            status = CTE_DECODE_STATUS_STOPPED_IN_CALLBACK;
            fbreak;
        }
        if(!process->callbacks->on_string_end(process))
        {
            status = CTE_DECODE_STATUS_STOPPED_IN_CALLBACK;
            fbreak;
        }
        process->token_start = NULL;
    }

    action on_list_begin
    {
        KSLOG_TRACE("on_list_begin");
        if(!process->callbacks->on_list_begin(process))
        {
            status = CTE_DECODE_STATUS_STOPPED_IN_CALLBACK;
            fbreak;
        }
        process->token_start = NULL;
        fcall iterate_list;
    }

    action on_list_end
    {
        KSLOG_TRACE("on_list_end");
        if(!process->callbacks->on_list_end(process))
        {
            status = CTE_DECODE_STATUS_STOPPED_IN_CALLBACK;
            fbreak;
        }
        process->token_start = NULL;
        fret;
    }

    action on_map_begin
    {
        KSLOG_TRACE("on_map_begin");
        if(!process->callbacks->on_map_begin(process))
        {
            status = CTE_DECODE_STATUS_STOPPED_IN_CALLBACK;
            fbreak;
        }
        process->token_start = NULL;
        fcall iterate_map;
    }

    action on_map_end
    {
        KSLOG_TRACE("on_map_end");
        if(!process->callbacks->on_map_end(process))
        {
            status = CTE_DECODE_STATUS_STOPPED_IN_CALLBACK;
            fbreak;
        }
        process->token_start = NULL;
        fret;
    }

    action on_error
    {
        KSLOG_TRACE("on_error: [%c]", *p);
        handle_error(process, p);
        fbreak;
    }


    ############
    # Matchers #
    ############

    nil =
    (
        "nil" @on_nil
    );

    boolean =
    (
        "true" @on_true |
        "false" @on_false
    );

    number_significand_base_2 = 
    (
        '0' |
        (
            '1' @on_number_significand_digit
            ([0-1] @on_number_significand_digit)*
        )
    );

    number_significand_base_8 = 
    (
        '0' |
        (
            [1-7] @on_number_significand_digit
            ([0-7] @on_number_significand_digit)*
        )
    );

    number_significand_base_10 = 
    (
        '0' |
        (
            [1-9] @on_number_significand_digit
            (digit @on_number_significand_digit)*
        )
    );

    number_significand_base_16 = 
    (
        '0' |
        (
            [1-9a-f] @on_number_significand_digit
            ([0-9a-f] @on_number_significand_digit)*
        )
    );

    number_fractional =
    (
        '.' @on_number_dot
        (digit @on_number_significand_digit)+
        (
            'e'
            ('+' | '-' @on_number_exponent_negative)
            [1-9] @on_number_exponent_digit
            (digit @on_number_exponent_digit)*
        )?

    );

    number =
    (
        "nan" @on_number_nan |
        (
            ('-' @on_number_negative)?
            (
                ("inf" @on_number_infinity) |
                (
                    "d:" @on_number_decimal
                    number_significand_base_10
                    number_fractional
                ) |
                (
                    "b:" @on_number_base_2
                    number_significand_base_2
                ) |
                (
                    "o:" @on_number_base_8
                    number_significand_base_8
                ) |
                (
                    "h:" @on_number_base_16
                    number_significand_base_16
                ) |
                (
                    number_significand_base_10
                    number_fractional?
                )
            )
        )
    ) >on_number_begin %on_number_end;

    time =
    (
        (digit @on_time_year_digit){4,}
        '-'
        (
            (
                (digit @on_time_month_digit){2}
                '-'
                (digit @on_time_day_digit){2}
            ) |
            (
                ((digit @on_time_day_digit){3}) @on_time_doy
            )
        )
        'T'
        (digit @on_time_hour_digit){2}
        ':'
        (digit @on_time_minute_digit){2}
        ':'
        (digit @on_time_second_digit){2}
        (
            '.'
            (digit @on_time_microsecond_digit){1,6}
        )?
        'Z'
    ) >on_time_start %on_time_end;

    binary =
    (
        (
            ('h' @on_binary_type_hex) |
            ('s' @on_binary_type_safe85)
        )
        '/' %on_binary_begin
        ((any - '/') >on_binary_character)*
        '/' >on_binary_end
    );

    string_delimiter = '"';
    escape_initiator = '\\';
    escaped_character = 
    (
        escape_initiator
        (
            (string_delimiter @on_string_character_double_quote) |
            ('n' @on_string_character_newline) |
            ('r' @on_string_character_carriage_return) |
            ('t' @on_string_character_tab) |
            (escape_initiator @on_string_character_backslash) |
            (
                'x'
                ([0-9a-f]{2} %on_string_character_hex)
            ) |
            (
                'u'
                ([0-9a-f]{4} %on_string_character_unicode)
            )
        )
    );

    string =
    (
        string_delimiter %on_string_begin
        (
            ((any - (string_delimiter | escape_initiator)) >on_string_character) |
            escaped_character
        )*
        string_delimiter >on_string_end
    );

    comment =
    (
        "# "
        (any - ('\r' | '\n'))*
        ('\r' | '\n')
    );

    list = '[' @on_list_begin;
    map = '{' @on_map_begin;

    map_key = boolean | number | time | string | binary;

    value = map_key | list | map | nil;

    key_value = map_key space* '=' space* value;


    ############
    # Machines #
    ############

    iterate_list :=
    (
        comment*
        space*
        (
            comment*
            value
            (
                space+
                comment*
                value
            )*
        )?
        comment*
        space*
        comment*
        ']' @on_list_end
    )
    $!on_error
    $/on_error;

    iterate_map :=
    (
        comment*
        space*
        (
            comment*
            key_value
            (
                space+
                comment*
                key_value
            )*
        )?
        comment*
        space*
        comment*
        '}' @on_map_end
    )
    $!on_error
    $/on_error;

    main := comment* value? comment* $!on_error @/on_error;
}%%

%% write data;


int cte_decode_process_size(const int max_container_depth)
{
    // Silence warnings from ragel-produced globals
    (void)cte_en_main;
    (void)cte_en_iterate_map;
    (void)cte_en_iterate_list;
    (void)cte_error;
    (void)cte_first_final;

    cte_decode_process* dummy_process;
    return sizeof(cte_decode_process) + sizeof(dummy_process->stack[0]  ) * max_container_depth;
}

cte_decode_status cte_decode_begin(cte_decode_process* const process,
                                   const cte_decode_callbacks* const callbacks,
                                   void* const user_context,
                                   const int max_container_depth)
{
    KSLOG_TRACE("Call: process %p, callbacks %p, user_context %p", process, callbacks, user_context);
    KSLOG_DEBUG("Begin process");

    memset(process, 0, sizeof(cte_decode_process));
    process->callbacks = callbacks;
    process->user_context = user_context;
    process->max_container_depth = max_container_depth;

    %% write init;

    return CTE_DECODE_STATUS_OK;
}

void* cte_decode_get_user_context(cte_decode_process* const process)
{
    return process->user_context;
}


cte_decode_status cte_decode_feed_internal(cte_decode_process* const process,
                                  const char* const data_start,
                                  const int64_t byte_count,
                                  const bool is_eof)
{
    KSLOG_TRACE("Call: process %p, data_start %p, byte_count %ld, is_eof %d", process, data_start, byte_count, is_eof);
    KSLOG_DEBUG("Feed %ld bytes, eof %d", byte_count, is_eof);

    // Ragel variables
    const char* p = data_start;
    const char* pe = p + byte_count;
    const char* eof = is_eof ? pe : NULL;

    process->token_start = NULL;

    cte_decode_status status = CTE_DECODE_STATUS_OK;

    %% write exec;

    return status;
}

cte_decode_status cte_decode_feed(cte_decode_process* const process,
                                  const char* const data_start,
                                  int64_t* const byte_count)
{
    // TODO: Not touching byte count because I'm not text-to-binary decoding yet.
    return cte_decode_feed_internal(process, data_start, *byte_count, false);
}

cte_decode_status cte_decode_end(cte_decode_process* const process)
{
    KSLOG_TRACE("Call: process %p", process);
    // STOP_AND_EXIT_IF_IS_INSIDE_CONTAINER(process);
    // STOP_AND_EXIT_IF_IS_INSIDE_ARRAY(process);

    const char* empty = "";
    cte_decode_status status = cte_decode_feed_internal(process, empty, 0, true);
    if(status != CTE_DECODE_STATUS_OK)
    {
        return status;
    }

    KSLOG_DEBUG("Process ended successfully");
    return CTE_DECODE_STATUS_OK;
}


cte_decode_status cte_decode(const cte_decode_callbacks* const callbacks,
                             void* const user_context,
                             const char* const document,
                             const int64_t document_length,
                             const int max_container_depth)
{
    KSLOG_TRACE("Call: callbacks %p, document %p, document_length %d", callbacks, document, document_length);
    char decode_process_backing_store[cte_decode_process_size(max_container_depth)];
    cte_decode_process* process = (cte_decode_process*)decode_process_backing_store;
    cte_decode_status status = cte_decode_begin(process, callbacks, NULL, max_container_depth);
    if(status != CTE_DECODE_STATUS_OK)
    {
        return status;
    }

    status = cte_decode_feed(process, document, document_length);
    if(status != CTE_DECODE_STATUS_OK)
    {
        return status;
    }

    return cte_decode_end(process);
}
