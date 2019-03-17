#include "cte/cte.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"

#define QUOTE(str) #str
#define EXPAND_AND_QUOTE(str) QUOTE(str)

const char* cte_version()
{
    return EXPAND_AND_QUOTE(PROJECT_VERSION);
}

// bytes octets
// ----- --------------------------------------
// 1     0xxxxxxx            
// 2     110xxxxx  10xxxxxx        
// 3     1110xxxx  10xxxxxx  10xxxxxx    
// 4     11110xxx  10xxxxxx  10xxxxxx  10xxxxxx

typedef struct
{
    int bytes_remaining;
    int accumulator;
} utf8_context;

static bool validate_utf8(utf8_context* context, uint8_t ch)
{
    const uint8_t continuation_mask = 0xc0;
    const uint8_t continuation_match = 0x80;
    if(context->bytes_remaining > 0)
    {
        if((ch & continuation_mask) != continuation_match)
        {
            return false;
        }
        context->bytes_remaining--;
        context->accumulator = (context->accumulator << 6) | (ch & ~continuation_mask);
        return true;
    }

    const uint8_t initiator_mask = 0x80;
    const uint8_t initiator_match = 0x80;
    if((ch & initiator_mask) != initiator_match)
    {
        context->bytes_remaining = 0;
        context->accumulator = ch;
        return true;
    }

    const uint8_t initiator_2_byte_mask = 0xe0;
    const uint8_t initiator_2_byte_match = 0xc0;
    const uint8_t first_byte_2_byte_mask = 0x1f;
    if((ch & initiator_2_byte_mask) == initiator_2_byte_match)
    {
        context->bytes_remaining = 1;
        context->accumulator = ch & first_byte_2_byte_mask;
        return true;
    }

    const uint8_t initiator_3_byte_mask = 0xf0;
    const uint8_t initiator_3_byte_match = 0xe0;
    const uint8_t first_byte_3_byte_mask = 0x0f;
    if((ch & initiator_3_byte_mask) == initiator_3_byte_match)
    {
        context->bytes_remaining = 2;
        context->accumulator = ch & first_byte_3_byte_mask;
        return true;
    }

    const uint8_t initiator_4_byte_mask = 0xf8;
    const uint8_t initiator_4_byte_match = 0xf0;
    const uint8_t first_byte_4_byte_mask = 0x07;
    if((ch & initiator_4_byte_mask) == initiator_4_byte_match)
    {
        context->bytes_remaining = 3;
        context->accumulator = ch & first_byte_4_byte_mask;
        return true;
    }

    return false;
}

static bool validate_comment(int ch)
{
    if(ch >= 0x00 && ch <= 0x1f && ch != 0x09)
    {
        return false;
    }
    if(ch >= 0x7f && ch <= 0x9f)
    {
        return false;
    }
    if(ch == 0x2028 || ch == 0x2029)
    {
        return false;
    }

    return true;
}

bool cte_validate_string(const uint8_t* const start, const int64_t byte_count)
{
    KSLOG_DEBUG("start %p, byte_count %d", start, byte_count);
    const uint8_t* ptr = start;
    const uint8_t* const end = ptr + byte_count;
    utf8_context context = {0};

    while(ptr < end)
    {
        uint8_t ch = *ptr++;
        if(!validate_utf8(&context, ch))
        {
            KSLOG_DEBUG("UTF-8 validation failed");
            return false;
        }
    }
    return true;
}

bool cte_validate_comment(const uint8_t* const start, const int64_t byte_count)
{
    KSLOG_DEBUG("start %p, byte_count %d", start, byte_count);
    const uint8_t* ptr = start;
    const uint8_t* const end = ptr + byte_count;
    utf8_context context = {0};

    while(ptr < end)
    {
        uint8_t ch = *ptr++;
        if(!validate_utf8(&context, ch))
        {
            KSLOG_DEBUG("UTF-8 validation failed");
            return false;
        }
        if(context.bytes_remaining == 0)
        {
            if(!validate_comment(context.accumulator))
            {
                KSLOG_DEBUG("Comment validation failed");
                return false;
            }
        }
    }
    return true;
}

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

static int g_doy_feb_28 = 31 + 28;

static inline int is_leap_year(int year)
{
    return (year % 4 == 0) && (year % 100 != 0 || year % 400 == 0);
}

int cte_ymd_to_doy(int year, int month, int day)
{
    int days = g_days_to_the_month[month] + day;
    if(is_leap_year(year) && month > 2)
    {
        days++;
    }
    KSLOG_DEBUG("year %d, month %d, day %d, doy %d", year, month, day, days);
    return days;
}

void cte_doy_to_month_and_day(int year, int doy, int* month_out, int* day_out)
{
    if(is_leap_year(year) && doy > g_doy_feb_28)
    {
        doy--;
    }
    int month = 1;
    for(month = 1; month < 12; month++)
    {
        if(doy < g_days_to_the_month[month])
        {
            break;
        }
    }
    month--;
    *month_out = month;
    *day_out = doy - g_days_to_the_month[month];
    KSLOG_DEBUG("year %d, doy %d, month %d, day %d", year, doy, *month_out, *day_out);
}
