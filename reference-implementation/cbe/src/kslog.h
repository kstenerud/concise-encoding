//
//  KSLog.h
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

#ifndef HDR_KSLog_h
#define HDR_KSLog_h

/**
 * KSLog
 * =====
 *
 * A very basic async-safe logger.
 * Prints log entries to the console consisting of:
 * - Level (Error, Warn, Info, Debug, Trace)
 * - File
 * - Line
 * - Function
 * - Message
 *
 * Various parts can be configured via the preprocessor.
 *
 *
 * =====
 * USAGE
 * =====
 *
 * Set the default logging level in your preprocessor settings. You may choose:
 *
 *  - KSLog_MinLevel=KSLOG_LEVEL_NONE
 *  - KSLog_MinLevel=KSLOG_LEVEL_TRACE
 *  - KSLog_MinLevel=KSLOG_LEVEL_DEBUG
 *  - KSLog_MinLevel=KSLOG_LEVEL_INFO
 *  - KSLog_MinLevel=KSLOG_LEVEL_WARN
 *  - KSLog_MinLevel=KSLOG_LEVEL_ERROR
 *
 * The default is KSLOG_LEVEL_INFO.
 *
 * Anything below your specified log level will not be compiled or printed.
 * 
 *
 * Next, include the header file:
 *
 * #include <kslog/kslog.h>
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
 * "KSLog_LocalMinLevel" define. Note that it must be defined BEFORE
 * including KSLogger.h
 *
 * This will override KSLog_MinLevel for the local file only.
 *
 * Example:
 *
 *     // Configuration defines MUST be defined BEFORE including kslog.h
 *     #define KSLog_LocalMinLevel TRACE
 *     #import <kslog/kslog.h>
 *
 *
 * ===============
 * IMPORTANT NOTES
 * ===============
 *
 * The default logger is async-safe, but has a limited length because it
 * allocates on the stack. Max length can be configured by KSLog_MaxWriteSize,
 * and defaults to 1024.
 *
 * Since the default implementation is async-safe, it is not thread-safe (you
 * can't have both). If you need thread safety, use KSLOG_CustomLogger and
 * implement your own thread-safe functions.
 */



// =============
// Configuration
// =============

// You can configure KSLog's behavior by defining these preprocessor values
// during compliation. Defaults are shown below:

// Calls with log levels below this will not even be compiled into the source.
#ifndef KSLog_MinLevel
    #define KSLog_MinLevel KSLOG_LEVEL_INFO
#endif

// Overrides KSLog_MinLevel to set the log level at the current source file.
// You an even set this to KSLOG_LEVEL_NONE to disable all logging in the
// current source file.
#ifndef KSLog_LocalMinLevel
    // Define this to a log level before #include to override for the local
    // file. Example:
    //     #define KSLog_LocalMinLevel KSLOG_LEVEL_DEBUG
    //     #include <kslog/kslog.h>
#endif

// Maximum bytes allowed in a single log write. Anything larger will be cut off.
// Choose this value wisely, as it will be allocated on the stack.
// Has no effect if KSLOG_CustomLogger is 1.
#ifndef KSLog_MaxWriteSize
    #define KSLog_MaxWriteSize 1024
#endif

// The file descriptor to write log entries to.
// Has no effect if KSLOG_CustomLogger is 1.
#ifndef KSLog_FileDesriptor
    #define KSLog_FileDesriptor STDERR_FILENO
#endif

// Set this to 1 to use custom logger functions that you provide.
// You must implement kslog_write_log() and kslog_write_log_basic().
#ifndef KSLOG_CustomLogger
    #define KSLOG_CustomLogger 0
#endif



// ========
// Internal
// ========

#ifndef ANSI_EXTENSION
    #ifdef __GNUC__
        #define ANSI_EXTENSION __extension__
    #else
        #define ANSI_EXTENSION
    #endif
#endif

#define KSLOG_LEVEL_NONE  0
#define KSLOG_LEVEL_ERROR 1
#define KSLOG_LEVEL_WARN  2
#define KSLOG_LEVEL_INFO  3
#define KSLOG_LEVEL_DEBUG 4
#define KSLOG_LEVEL_TRACE 5


#ifdef KSLog_LocalMinLevel
    #undef KSLog_MinLevel
    #define KSLog_MinLevel KSLog_LocalMinLevel
#endif


#if KSLog_MinLevel > KSLOG_LEVEL_NONE

#if KSLOG_CustomLogger

void kslog_write_log(
    const char* level,
    const char* file,
    unsigned int line,
    const char* function,
    const unsigned char* binary_data,
    int byte_count, 
    const char* fmt, ...);

void kslog_write_log_basic(
    const unsigned char* binary_data,
    int byte_count,
    const char* fmt, ...);

#else

static void kslog_write_log(
    const char* level,
    const char* file,
    unsigned int line,
    const char* function,
    const unsigned char* binary_data,
    int byte_count, 
    const char* fmt, ...);

static void kslog_write_log_basic(
    const unsigned char* binary_data,
    int byte_count,
    const char* fmt, ...);

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static void kslog_write_string(const char* str)
{
    size_t bytesToWrite = strlen(str);
    const char* pos = str;
    while(bytesToWrite > 0)
    {
        ssize_t bytesWritten = write(KSLog_FileDesriptor, pos, bytesToWrite);
        if(bytesWritten == -1)
        {
            return;
        }
        bytesToWrite -= (size_t)bytesWritten;
        pos += bytesWritten;
    }
}

static void kslog_write_newline()
{
    write(KSLog_FileDesriptor, "\n", 1);
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

    write(KSLog_FileDesriptor, "[", 1);
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
        write(KSLog_FileDesriptor, buffer, byte_count);
    }
    write(KSLog_FileDesriptor, "]", 1);
}

static void kslog_write_varargs(const char* fmt, va_list args)
{
    char buffer[KSLog_MaxWriteSize];
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

static const char* kslog_last_path_entry(const char* path)
{
    const char* lastFile = strrchr(path, '/');
    return lastFile == 0 ? path : lastFile + 1;
}

static void kslog_write_log_basic(
    const unsigned char* binary_data,
    int byte_count,
    const char* fmt, ...)
{
    if(fmt != NULL && *fmt != 0)
    {
        va_list args;
        va_start(args,fmt);
        kslog_write_varargs(fmt, args);
        va_end(args);

        // Prevents "unused function" warning for kslog_write_log().
        (void)kslog_write_log;
    }
    kslog_write_hex(binary_data, byte_count);
    kslog_write_newline();
}

static void kslog_write_log(
    const char* level,
    const char* file,
    unsigned int line,
    const char* function,
    const unsigned char* binary_data,
    int byte_count, 
    const char* fmt, ...)
{
    kslog_write_wildcard("%s: %s (%u): %s: ",
             level, kslog_last_path_entry(file), line, function);
    if(fmt != NULL && *fmt != 0)
    {
        va_list args;
        va_start(args,fmt);
        kslog_write_varargs(fmt, args);
        va_end(args);

        // Avoid "unused function" warning.
        (void)kslog_write_log_basic;
    }
    kslog_write_hex(binary_data, byte_count);
    kslog_write_newline();
}

#endif // !KSLOG_CustomLogger

#endif // KSLog_MinLevel > KSLOG_LEVEL_NONE

#define KSLOG_BASIC kslog_write_log_basic
#define indirect_KSLOG_FULL kslog_write_log
#define KSLOG_FULL(LEVEL, BINARY_DATA, BYTE_COUNT, ...) \
    ANSI_EXTENSION indirect_KSLOG_FULL(LEVEL, \
        __FILE__, __LINE__, __PRETTY_FUNCTION__, \
        (uint8_t*)BINARY_DATA, BYTE_COUNT, ##__VA_ARGS__)



// ==========
// Public API
// ==========

/** Log an error.
 * Normal version prints out full context. Basic version prints directly.
 *
 * @param BINARY_DATA Data to be printed as hex.
 * @param BYTE_COUNT Number of bytes to print.
 * @param ... The format specifier, followed by its arguments.
 */
#if KSLog_MinLevel >= KSLOG_LEVEL_ERROR
    #define KSLOG_DATA_ERROR(BINARY_DATA, BYTE_COUNT, ...) KSLOG_FULL("ERROR", BINARY_DATA, BYTE_COUNT, ##__VA_ARGS__)
    #define KSLOGBASIC_DATA_ERROR(BINARY_DATA, BYTE_COUNT, ...) KSLOG_BASIC(BINARY_DATA, BYTE_COUNT, ##__VA_ARGS__)
#else
    #define KSLOG_DATA_ERROR(BINARY_DATA, BYTE_COUNT, ...)
    #define KSLOGBASIC_DATA_ERROR(BINARY_DATA, BYTE_COUNT, ...)
#endif
#define KSLOG_ERROR(...) KSLOG_DATA_ERROR(NULL, 0, ##__VA_ARGS__)
#define KSLOGBASIC_ERROR(...) KSLOGBASIC_DATA_ERROR(NULL, 0, ##__VA_ARGS__)

/** Log a warning.
 * Normal version prints out full context. Basic version prints directly.
 *
 * @param BINARY_DATA Data to be printed as hex.
 * @param BYTE_COUNT Number of bytes to print.
 * @param FMT The format specifier, followed by its arguments.
 */
#if KSLog_MinLevel >= KSLOG_LEVEL_WARN
    #define KSLOG_DATA_WARN(BINARY_DATA, BYTE_COUNT, ...) KSLOG_FULL("WARN", BINARY_DATA, BYTE_COUNT, ##__VA_ARGS__)
    #define KSLOGBASIC_DATA_WARN(BINARY_DATA, BYTE_COUNT, ...) KSLOG_BASIC(BINARY_DATA, BYTE_COUNT, ##__VA_ARGS__)
#else
    #define KSLOG_DATA_WARN(BINARY_DATA, BYTE_COUNT, ...)
    #define KSLOGBASIC_DATA_WARN(BINARY_DATA, BYTE_COUNT, ...)
#endif
#define KSLOG_WARN(...) KSLOG_DATA_WARN(NULL, 0, ##__VA_ARGS__)
#define KSLOGBASIC_WARN(...) KSLOGBASIC_DATA_WARN(NULL, 0, ##__VA_ARGS__)

/** Log an info message.
 * Normal version prints out full context. Basic version prints directly.
 *
 * @param BINARY_DATA Data to be printed as hex.
 * @param BYTE_COUNT Number of bytes to print.
 * @param FMT The format specifier, followed by its arguments.
 */
#if KSLog_MinLevel >= KSLOG_LEVEL_INFO
    #define KSLOG_DATA_INFO(BINARY_DATA, BYTE_COUNT, ...) KSLOG_FULL("INFO", BINARY_DATA, BYTE_COUNT, ##__VA_ARGS__)
    #define KSLOGBASIC_DATA_INFO(BINARY_DATA, BYTE_COUNT, ...) KSLOG_BASIC(BINARY_DATA, BYTE_COUNT, ##__VA_ARGS__)
#else
    #define KSLOG_DATA_INFO(BINARY_DATA, BYTE_COUNT, ...)
    #define KSLOGBASIC_DATA_INFO(BINARY_DATA, BYTE_COUNT, ...)
#endif
#define KSLOG_INFO(...) KSLOG_DATA_INFO(NULL, 0, ##__VA_ARGS__)
#define KSLOGBASIC_INFO(...) KSLOGBASIC_DATA_INFO(NULL, 0, ##__VA_ARGS__)

/** Log a debug message.
 * Normal version prints out full context. Basic version prints directly.
 *
 * @param BINARY_DATA Data to be printed as hex.
 * @param BYTE_COUNT Number of bytes to print.
 * @param FMT The format specifier, followed by its arguments.
 */
#if KSLog_MinLevel >= KSLOG_LEVEL_DEBUG
    #define KSLOG_DATA_DEBUG(BINARY_DATA, BYTE_COUNT, ...) KSLOG_FULL("DEBUG", BINARY_DATA, BYTE_COUNT, ##__VA_ARGS__)
    #define KSLOGBASIC_DATA_DEBUG(BINARY_DATA, BYTE_COUNT, ...) KSLOG_BASIC(BINARY_DATA, BYTE_COUNT, ##__VA_ARGS__)
#else
    #define KSLOG_DATA_DEBUG(BINARY_DATA, BYTE_COUNT, ...)
    #define KSLOGBASIC_DATA_DEBUG(BINARY_DATA, BYTE_COUNT, ...)
#endif
#define KSLOG_DEBUG(...) KSLOG_DATA_DEBUG(NULL, 0, ##__VA_ARGS__)
#define KSLOGBASIC_DEBUG(...) KSLOGBASIC_DATA_DEBUG(NULL, 0, ##__VA_ARGS__)

/** Log a trace message.
 * Normal version prints out full context. Basic version prints directly.
 *
 * @param BINARY_DATA Data to be printed as hex.
 * @param BYTE_COUNT Number of bytes to print.
 * @param FMT The format specifier, followed by its arguments.
 */
#if KSLog_MinLevel >= KSLOG_LEVEL_TRACE
    #define KSLOG_DATA_TRACE(BINARY_DATA, BYTE_COUNT, ...) KSLOG_FULL("TRACE", BINARY_DATA, BYTE_COUNT, ##__VA_ARGS__)
    #define KSLOGBASIC_DATA_TRACE(BINARY_DATA, BYTE_COUNT, ...) KSLOG_BASIC(BINARY_DATA, BYTE_COUNT, ##__VA_ARGS__)
#else
    #define KSLOG_DATA_TRACE(BINARY_DATA, BYTE_COUNT, ...)
    #define KSLOGBASIC_DATA_TRACE(BINARY_DATA, BYTE_COUNT, ...)
#endif
#define KSLOG_TRACE(...) KSLOG_DATA_TRACE(NULL, 0, ##__VA_ARGS__)
#define KSLOGBASIC_TRACE(...) KSLOGBASIC_DATA_TRACE(NULL, 0, ##__VA_ARGS__)


#undef KSLOG_LEVEL_ERROR
#undef KSLOG_LEVEL_WARNING
#undef KSLOG_LEVEL_INFO
#undef KSLOG_LEVEL_DEBUG
#undef KSLOG_LEVEL_TRACE

#endif // HDR_KSLog_h
