#include "cbe/cbe.h"
#include "cbe_version.h"
#include "cbe_internal.h"

const char* cbe_version()
{
    return CBE_VERSION;
}

int64_t cbe_new_time(int year, int day, int hour, int minute, int second, int microsecond)
{
    return ((int64_t)year    << TIME_BITSHIFT_YEAR)   |
           ((uint64_t)day    << TIME_BITSHIFT_DAY)    |
           ((uint64_t)hour   << TIME_BITSHIFT_HOUR)   |
           ((uint64_t)minute << TIME_BITSHIFT_MINUTE) |
           ((uint64_t)second << TIME_BITSHIFT_SECOND) |
           microsecond;
}

int cbe_time_get_year(int64_t time)
{
    return time >> TIME_BITSHIFT_YEAR;
}

int cbe_time_get_day(int64_t time)
{
    return (time >> TIME_BITSHIFT_DAY) & TIME_MASK_DAY;
}

int cbe_time_get_hour(int64_t time)
{
    return (time >> TIME_BITSHIFT_HOUR) & TIME_MASK_HOUR;
}

int cbe_time_get_minute(int64_t time)
{
    return (time >> TIME_BITSHIFT_MINUTE) & TIME_MASK_MINUTE;
}

int cbe_time_get_second(int64_t time)
{
    return (time >> TIME_BITSHIFT_SECOND) & TIME_MASK_SECOND;
}

int cbe_time_get_microsecond(int64_t time)
{
    return time & TIME_MASK_MICROSECOND;
}
