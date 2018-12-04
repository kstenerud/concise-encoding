//
//  KSLogger.h
//
//  Created by Karl Stenerud on 2011-06-25.
//
//  Copyright (c) 2011 Karl Stenerud. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall remain in place
// in this source code.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#ifndef HDR_KSLoggerCommon_h
#define HDR_KSLoggerCommon_h


/**
 * KSLogger
 * ========
 *
 * Prints log entries to the console consisting of:
 * - Level (Error, Warn, Info, Debug, Trace)
 * - File
 * - Line
 * - Function
 * - Message
 *
 * Can set the minimum logging level in the preprocessor.
 *
 *
 * =====
 * USAGE
 * =====
 *
 * Set the default logging level in your preprocessor settings. You may choose
 * NONE, TRACE, DEBUG, INFO, WARN, ERROR. If nothing is set, it defaults to INFO.
 *
 *     KSLogger_Level=WARN
 *
 * Anything below your specified log level will not be printed.
 * 
 *
 * Next, include the header file:
 *
 * #include "KSLogger.h"
 *
 *
 * Next, call the logger functions from your code:
 *
 * Code:
 *     KSLOG_ERROR("Some error message");
 *
 * Prints:
 *     ERROR: SomeFile.c (21): some_function: Some error message 
 *
 * Code:
 *     KSLOG_INFO("The value is %d", someInteger);
*
 * Prints:
 *     INFO : SomeFile.c (22): some_function: The value is 10
 *
 *
 * The "BASIC" versions output only what you supply:
 *
 * Code:
 *     KSLOGBASIC_ERROR("A basic log entry");
 *
 * Prints:
 *     A basic log entry
 *
 *
 * =============
 * LOCAL LOGGING
 * =============
 *
 * You can control logging messages at the local file level using the
 * "KSLogger_LocalLevel" define. Note that it must be defined BEFORE
 * including KSLogger.h
 *
 * The KSLOG_XX() and KSLOGBASIC_XX() macros will print out based on the LOWER
 * of KSLogger_Level and KSLogger_LocalLevel, so if KSLogger_Level is DEBUG
 * and KSLogger_LocalLevel is TRACE, it will print all the way down to the trace
 * level for that file, and to the debug level everywhere else.
 *
 * Example:
 *
 *     // KSLogger_LocalLevel, MUST be defined BEFORE including KSLogger.h
 *     #define KSLogger_LocalLevel TRACE
 *     #import "KSLogger.h"
 *
 *
 * ===============
 * IMPORTANT NOTES
 * ===============
 *
 * The logger is async-safe, but is limited in how big of a log message it can
 * print. By default it is 1024 bytes, but the preprocessor define
 * KSLogger_BufferSize can set the size (only during compilation of KSLogger).
 */


/* Back up any existing defines by the same name */
#ifdef NONE
    #define KSLOG_BAK_NONE NONE
#undef NONE
#endif
#ifdef ERROR
    #define KSLOG_BAK_ERROR ERROR
#undef ERROR
#endif
#ifdef WARN
    #define KSLOG_BAK_WARN WARN
#undef WARN
#endif
#ifdef INFO
    #define KSLOG_BAK_INFO INFO
#undef INFO
#endif
#ifdef DEBUG
    #define KSLOG_BAK_DEBUG DEBUG
#undef DEBUG
#endif
#ifdef TRACE
    #define KSLOG_BAK_TRACE TRACE
#undef TRACE
#endif
#define NONE  0
#define ERROR 1
#define WARN  2
#define INFO  3
#define DEBUG 4
#define TRACE 5



// =====================
// Default Configuration
// =====================

#ifndef KSLogger_Level
    #define KSLogger_Level INFO
#endif

#ifndef KSLogger_LocalLevel
    #define KSLogger_LocalLevel NONE
#endif

#ifndef KSLogger_BufferSize
    #define KSLogger_BufferSize 1024
#endif



// ========
// Internal
// ========

#if KSLogger_LocalLevel > KSLogger_Level
    #undef KSLogger_Level
    #define KSLogger_Level KSLogger_LocalLevel
#endif


#if KSLogger_Level > NONE

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static const char* kslog_last_path_entry(const char* path)
{
    const char* lastFile = strrchr(path, '/');
    return lastFile == 0 ? path : lastFile + 1;
}

static void kslog_write_string(const char* str)
{
    size_t bytesToWrite = strlen(str);
    const char* pos = str;
    while(bytesToWrite > 0)
    {
        ssize_t bytesWritten = write(STDOUT_FILENO, pos, bytesToWrite);
        if(bytesWritten == -1)
        {
            return;
        }
        bytesToWrite -= (size_t)bytesWritten;
        pos += bytesWritten;
    }
}

static void kslog_write_hex(const unsigned char* ptr, const int length)
{
    if(length <= 0)
    {
        return;
    }

    static char hex_table[] =
    {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
    };

    write(STDOUT_FILENO, "[", 1);
    for(int i = 0; i < length; i++)
    {
        char buffer[3];
        int byte_count = 2;
        unsigned char byte = ptr[i];
        buffer[0] = hex_table[byte >> 4];
        buffer[1] = hex_table[byte & 15];
        if(i + 1 < length)
        {
            buffer[2] = ' ';
            byte_count++;
        }
        write(STDOUT_FILENO, buffer, byte_count);
    }
    write(STDOUT_FILENO, "]", 1);
}

static void kslog_write_varargs(const char* fmt, va_list args)
{
    char buffer[KSLogger_BufferSize];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    kslog_write_string(buffer);
}

static void kslog_write_wildcard(const char* fmt, ...)
{
    va_list args;
    va_start(args,fmt);
    kslog_write_varargs(fmt, args);
    va_end(args);
}

static void kslog_write_log_contents(
                    const unsigned char* binary_data,
                    int byte_count,
                    const char* fmt,
                    va_list args)
{
    kslog_write_varargs(fmt, args);
    kslog_write_hex(binary_data, byte_count);
    write(STDOUT_FILENO, "\n", 1);
}

static void kslog_write_log(const char* level,
               const char* file,
               unsigned int line,
               const char* function,
               const unsigned char* binary_data,
               int byte_count, 
               const char* fmt, ...);

static void kslog_write_log_basic(const unsigned char* binary_data,
                     int byte_count,
                     const char* fmt, ...)
{
    if(*fmt != 0)
    {
        va_list args;
        va_start(args,fmt);
        kslog_write_log_contents(binary_data, byte_count, fmt, args);
        va_end(args);

        // Avoid "unused function" warning.
        (void)kslog_write_log;
    }
}

static void kslog_write_log(const char* level,
               const char* file,
               unsigned int line,
               const char* function,
               const unsigned char* binary_data,
               int byte_count, 
               const char* fmt, ...)
{
    if(*fmt != 0)
    {
        kslog_write_wildcard("%s: %s (%u): %s: ",
                 level, kslog_last_path_entry(file), line, function);

        va_list args;
        va_start(args,fmt);
        kslog_write_log_contents(binary_data, byte_count, fmt, args);
        va_end(args);

        // Avoid "unused function" warning.
        (void)kslog_write_log_basic;
    }
}

#endif

#define KSLOG_BASIC kslog_write_log_basic
#define indirect_KSLOG_FULL kslog_write_log
#define KSLOG_FULL(LEVEL, BINARY_DATA, BYTE_COUNT, FMT, ...) \
    indirect_KSLOG_FULL(LEVEL, \
                        __FILE__, \
                        __LINE__, \
                        __PRETTY_FUNCTION__, \
                        (uint8_t*)BINARY_DATA, \
                        BYTE_COUNT, \
                        FMT, \
                        ##__VA_ARGS__)



// ==========
// Public API
// ==========

/** Log an error.
 * Normal version prints out full context. Basic version prints directly.
 *
 * @param BINARY_DATA Data to be printed as hex.
 * @param BYTE_COUNT Number of bytes to print.
 * @param FMT The format specifier, followed by its arguments.
 */
#if KSLogger_Level >= ERROR
    #define KSLOG_DATA_ERROR(BINARY_DATA, BYTE_COUNT, FMT, ...) KSLOG_FULL("ERROR", BINARY_DATA, BYTE_COUNT, FMT, ##__VA_ARGS__)
    #define KSLOGBASIC_DATA_ERROR(BINARY_DATA, BYTE_COUNT, FMT, ...) KSLOG_BASIC(BINARY_DATA, BYTE_COUNT, FMT, ##__VA_ARGS__)
#else
    #define KSLOG_DATA_ERROR(BINARY_DATA, BYTE_COUNT, FMT, ...)
    #define KSLOGBASIC_DATA_ERROR(BINARY_DATA, BYTE_COUNT, FMT, ...)
#endif
#define KSLOG_ERROR(FMT, ...) KSLOG_DATA_ERROR(NULL, 0, FMT, ##__VA_ARGS__)
#define KSLOGBASIC_ERROR(FMT, ...) KSLOGBASIC_DATA_ERROR(NULL, 0, FMT, ##__VA_ARGS__)

/** Log a warning.
 * Normal version prints out full context. Basic version prints directly.
 *
 * @param BINARY_DATA Data to be printed as hex.
 * @param BYTE_COUNT Number of bytes to print.
 * @param FMT The format specifier, followed by its arguments.
 */
#if KSLogger_Level >= WARN
    #define KSLOG_DATA_WARN(BINARY_DATA, BYTE_COUNT, FMT, ...) KSLOG_FULL("WARN", BINARY_DATA, BYTE_COUNT, FMT, ##__VA_ARGS__)
    #define KSLOGBASIC_DATA_WARN(BINARY_DATA, BYTE_COUNT, FMT, ...) KSLOG_BASIC(BINARY_DATA, BYTE_COUNT, FMT, ##__VA_ARGS__)
#else
    #define KSLOG_DATA_WARN(BINARY_DATA, BYTE_COUNT, FMT, ...)
    #define KSLOGBASIC_DATA_WARN(BINARY_DATA, BYTE_COUNT, FMT, ...)
#endif
#define KSLOG_WARN(FMT, ...) KSLOG_DATA_WARN(NULL, 0, FMT, ##__VA_ARGS__)
#define KSLOGBASIC_WARN(FMT, ...) KSLOGBASIC_DATA_WARN(NULL, 0, FMT, ##__VA_ARGS__)

/** Log an info message.
 * Normal version prints out full context. Basic version prints directly.
 *
 * @param BINARY_DATA Data to be printed as hex.
 * @param BYTE_COUNT Number of bytes to print.
 * @param FMT The format specifier, followed by its arguments.
 */
#if KSLogger_Level >= INFO
    #define KSLOG_DATA_INFO(BINARY_DATA, BYTE_COUNT, FMT, ...) KSLOG_FULL("INFO", BINARY_DATA, BYTE_COUNT, FMT, ##__VA_ARGS__)
    #define KSLOGBASIC_DATA_INFO(BINARY_DATA, BYTE_COUNT, FMT, ...) KSLOG_BASIC(BINARY_DATA, BYTE_COUNT, FMT, ##__VA_ARGS__)
#else
    #define KSLOG_DATA_INFO(BINARY_DATA, BYTE_COUNT, FMT, ...)
    #define KSLOGBASIC_DATA_INFO(BINARY_DATA, BYTE_COUNT, FMT, ...)
#endif
#define KSLOG_INFO(FMT, ...) KSLOG_DATA_INFO(NULL, 0, FMT, ##__VA_ARGS__)
#define KSLOGBASIC_INFO(FMT, ...) KSLOGBASIC_DATA_INFO(NULL, 0, FMT, ##__VA_ARGS__)

/** Log a debug message.
 * Normal version prints out full context. Basic version prints directly.
 *
 * @param BINARY_DATA Data to be printed as hex.
 * @param BYTE_COUNT Number of bytes to print.
 * @param FMT The format specifier, followed by its arguments.
 */
#if KSLogger_Level >= DEBUG
    #define KSLOG_DATA_DEBUG(BINARY_DATA, BYTE_COUNT, FMT, ...) KSLOG_FULL("DEBUG", BINARY_DATA, BYTE_COUNT, FMT, ##__VA_ARGS__)
    #define KSLOGBASIC_DATA_DEBUG(BINARY_DATA, BYTE_COUNT, FMT, ...) KSLOG_BASIC(BINARY_DATA, BYTE_COUNT, FMT, ##__VA_ARGS__)
#else
    #define KSLOG_DATA_DEBUG(BINARY_DATA, BYTE_COUNT, FMT, ...)
    #define KSLOGBASIC_DATA_DEBUG(BINARY_DATA, BYTE_COUNT, FMT, ...)
#endif
#define KSLOG_DEBUG(FMT, ...) KSLOG_DATA_DEBUG(NULL, 0, FMT, ##__VA_ARGS__)
#define KSLOGBASIC_DEBUG(FMT, ...) KSLOGBASIC_DATA_DEBUG(NULL, 0, FMT, ##__VA_ARGS__)

/** Log a trace message.
 * Normal version prints out full context. Basic version prints directly.
 *
 * @param BINARY_DATA Data to be printed as hex.
 * @param BYTE_COUNT Number of bytes to print.
 * @param FMT The format specifier, followed by its arguments.
 */
#if KSLogger_Level >= TRACE
    #define KSLOG_DATA_TRACE(BINARY_DATA, BYTE_COUNT, FMT, ...) KSLOG_FULL("TRACE", BINARY_DATA, BYTE_COUNT, FMT, ##__VA_ARGS__)
    #define KSLOGBASIC_DATA_TRACE(BINARY_DATA, BYTE_COUNT, FMT, ...) KSLOG_BASIC(BINARY_DATA, BYTE_COUNT, FMT, ##__VA_ARGS__)
#else
    #define KSLOG_DATA_TRACE(BINARY_DATA, BYTE_COUNT, FMT, ...)
    #define KSLOGBASIC_DATA_TRACE(BINARY_DATA, BYTE_COUNT, FMT, ...)
#endif
#define KSLOG_TRACE(FMT, ...) KSLOG_DATA_TRACE(NULL, 0, FMT, ##__VA_ARGS__)
#define KSLOGBASIC_TRACE(FMT, ...) KSLOGBASIC_DATA_TRACE(NULL, 0, FMT, ##__VA_ARGS__)



/* Restore any backed up defines */
#undef ERROR
#ifdef KSLOG_BAK_ERROR
    #define ERROR KSLOG_BAK_ERROR
    #undef KSLOG_BAK_ERROR
#endif
#undef WARNING
#ifdef KSLOG_BAK_WARN
    #define WARNING KSLOG_BAK_WARN
    #undef KSLOG_BAK_WARN
#endif
#undef INFO
#ifdef KSLOG_BAK_INFO
    #define INFO KSLOG_BAK_INFO
    #undef KSLOG_BAK_INFO
#endif
#undef DEBUG
    #ifdef KSLOG_BAK_DEBUG
    #define DEBUG KSLOG_BAK_DEBUG
#undef KSLOG_BAK_DEBUG
#endif
#undef TRACE
#ifdef KSLOG_BAK_TRACE
    #define DEBUG KSLOG_BAK_TRACE
    #undef KSLOG_BAK_TRACE
#endif


#endif // HDR_KSLoggerCommon_h
