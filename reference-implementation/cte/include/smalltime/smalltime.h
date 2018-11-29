/*
 * Smalltime
 * =========
 *
 * A simple and convenient date & time format in 64 bits.
 *
 *
 * Features
 * --------
 *
 *  * Encodes a complete date & time into a 64-bit signed integer.
 *  * Maintenance-free (no human managed tables to update).
 *  * Easily converts to human readable fields.
 *  * Supports hundreds of thousands of years.
 *  * Supports time units to the microsecond.
 *  * Supports leap years and leap seconds.
 *  * Values are always in timezone zero.
 *  * Encoded values are comparable.
 *
 *
 * License
 * -------
 *
 * Copyright 2018 Karl Stenerud
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifndef KS_smalltime_H
#define KS_smalltime_H
#ifdef __cplusplus 
extern "C" {
#endif

#include <stdint.h>


typedef int64_t smalltime;


// Internal defines. These will be undef'd at the end of the header.
#define SMALLTIME_BITSHIFT_YEAR    46
#define SMALLTIME_BITSHIFT_DAY     37
#define SMALLTIME_BITSHIFT_HOUR    32
#define SMALLTIME_BITSHIFT_MINUTE  26
#define SMALLTIME_BITSHIFT_SECOND  20

#define SMALLTIME_MASK_YEAR        (0x3ffffLL << SMALLTIME_BITSHIFT_YEAR)
#define SMALLTIME_MASK_DAY         (0x1ffLL   << SMALLTIME_BITSHIFT_DAY)
#define SMALLTIME_MASK_HOUR        (0x1fLL    << SMALLTIME_BITSHIFT_HOUR)
#define SMALLTIME_MASK_MINUTE      (0x3fLL    << SMALLTIME_BITSHIFT_MINUTE)
#define SMALLTIME_MASK_SECOND      (0x3fLL    << SMALLTIME_BITSHIFT_SECOND)
#define SMALLTIME_MASK_MICROSECOND 0xfffff



/**
 * Create a new time value, encoding the fields into a 64-bit signed integer.
 * Note: This function does NOT validate input! Make sure your source values are correct!
 *
 * @param year The year (-131072 - 131071). Note: 1 = 1 AD, 0 = 1 BC, -1 = 2 BC, ...
 * @param day The day of the year, allowing for leap year (1 - 366).
 * @param hour The hour of the day (0 - 23).
 * @param minute The minute of the hour (0 - 59).
 * @param second The second of the minute, allowing for leap second (0 - 60).
 * @param microsecond The microsecond of the second (0 - 999999).
 * @return A new time value.
 */
static inline smalltime smalltime_new(int year, int day, int hour, int minute, int second, int microsecond)
{
    return ((smalltime)year   << SMALLTIME_BITSHIFT_YEAR)   |
           ((smalltime)day    << SMALLTIME_BITSHIFT_DAY)    |
           ((smalltime)hour   << SMALLTIME_BITSHIFT_HOUR)   |
           ((smalltime)minute << SMALLTIME_BITSHIFT_MINUTE) |
           ((smalltime)second << SMALLTIME_BITSHIFT_SECOND) |
           microsecond;
}

/**
 * Get the year component from a time value.
 *
 * @param time The time value.
 * @return the year component.
 */
static inline int smalltime_get_year(smalltime time)
{
    return time >> SMALLTIME_BITSHIFT_YEAR;
}

/**
 * Set the year component of a time value.
 * Valid range is -131072 to 131071.
 * Note: There is no 0 AD or BC, so BC years are adjusted by 1:
 *       1 = 1 AD, 0 = 1 BC, -1 = 2 BC, ...
 * Note: Input is NOT validated!
 *
 * @param time The time value.
 * @param year The year component.
 * @return the new time value.
 */
static inline smalltime smalltime_set_year(smalltime time, int year)
{
    return (time & ~SMALLTIME_MASK_YEAR) | ((smalltime)year << SMALLTIME_BITSHIFT_YEAR);
}

/**
 * Get the day component from a time value.
 *
 * @param time The time value.
 * @return the day component.
 */
static inline int smalltime_get_day(smalltime time)
{
    return (time & SMALLTIME_MASK_DAY) >> SMALLTIME_BITSHIFT_DAY;
}

/**
 * Set the day component of a time value.
 * Valid range is 1 to 366 (ordinal date).
 * The value 366 is to support leap years.
 * Note: Input is NOT validated!
 *
 * @param time The time value.
 * @param day The day component.
 * @return the new time value.
 */
static inline smalltime smalltime_set_day(smalltime time, int day)
{
    return (time & ~SMALLTIME_MASK_DAY) | ((smalltime)day << SMALLTIME_BITSHIFT_DAY);
}

/**
 * Get the hour component from a time value.
 *
 * @param time The time value.
 * @return the hour component.
 */
static inline int smalltime_get_hour(smalltime time)
{
    return (time & SMALLTIME_MASK_HOUR) >> SMALLTIME_BITSHIFT_HOUR;
}

/**
 * Set the hour component of a time value.
 * Valid range is 0 to 23.
 * Note: Input is NOT validated!
 *
 * @param time The time value.
 * @param hour The hour component.
 * @return the new time value.
 */
static inline smalltime smalltime_set_hour(smalltime time, int hour)
{
    return (time & ~SMALLTIME_MASK_HOUR) | ((smalltime)hour << SMALLTIME_BITSHIFT_HOUR);
}

/**
 * Get the minute component from a time value.
 *
 * @param time The time value.
 * @return the minute component.
 */
static inline int smalltime_get_minute(smalltime time)
{
    return (time & SMALLTIME_MASK_MINUTE) >> SMALLTIME_BITSHIFT_MINUTE;
}

/**
 * Set the minute component of a time value.
 * Valid range is 0-59.
 * Note: Input is NOT validated!
 *
 * @param time The time value.
 * @param minute The minute component.
 * @return the new time value.
 */
static inline smalltime smalltime_set_minute(smalltime time, int minute)
{
    return (time & ~SMALLTIME_MASK_MINUTE) | ((smalltime)minute << SMALLTIME_BITSHIFT_MINUTE);
}

/**
 * Get the second component from a time value.
 *
 * @param time The time value.
 * @return the second component.
 */
static inline int smalltime_get_second(smalltime time)
{
    return (time & SMALLTIME_MASK_SECOND) >> SMALLTIME_BITSHIFT_SECOND;
}

/**
 * Set the second component of a time value.
 * Valid range is 0-60.
 * The value 60 is to support leap seconds.
 * Note: Input is NOT validated!
 *
 * @param time The time value.
 * @param second The second component.
 * @return the new time value.
 */
static inline smalltime smalltime_set_second(smalltime time, int second)
{
    return (time & ~SMALLTIME_MASK_SECOND) | ((smalltime)second << SMALLTIME_BITSHIFT_SECOND);
}

/**
 * Get the microsecond component from a time value.
 *
 * @param time The time value.
 * @return the microsecond component.
 */
static inline int smalltime_get_microsecond(smalltime time)
{
    return time & SMALLTIME_MASK_MICROSECOND;
}

/**
 * Set the microsecond component of a time value.
 * Valid range is 0-999999.
 * Note: Input is NOT validated!
 *
 * @param time The time value.
 * @param microsecond The microsecond component.
 * @return the new time value.
 */
static inline smalltime smalltime_set_microsecond(smalltime time, int microsecond)
{
    return (time & ~SMALLTIME_MASK_MICROSECOND) | microsecond;
}


#undef SMALLTIME_BITSHIFT_YEAR
#undef SMALLTIME_BITSHIFT_DAY
#undef SMALLTIME_BITSHIFT_HOUR
#undef SMALLTIME_BITSHIFT_MINUTE
#undef SMALLTIME_BITSHIFT_SECOND

#undef SMALLTIME_MASK_DAY
#undef SMALLTIME_MASK_HOUR
#undef SMALLTIME_MASK_MINUTE
#undef SMALLTIME_MASK_SECOND
#undef SMALLTIME_MASK_MICROSECOND


#ifdef __cplusplus 
}
#endif
#endif // KS_smalltime_H
