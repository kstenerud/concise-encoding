//
//  KSLoggerCommon.h
//
//  Created by Karl Stenerud on 11-06-25.
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
 * Set the log level in your "Preprocessor Macros" build setting. You may choose
 * TRACE, DEBUG, INFO, WARN, ERROR. If nothing is set, it defaults to INFO.
 *
 * KSLogger_Level=WARN
 *
 * Anything below that log level will not be printed.
 * 
 *
 * Next, include the correct header file:
 *
 * #include "KSLogger.h"
 *
 *
 * Next, call the logger functions from your code:
 *
 * Code:
 *    KSLOG_ERROR(@"Some error message");
 *
 * Prints:
 *    2011-09-25 05:41:01.379 TestApp[4439:f803] ERROR: SomeClass.m (21): -[SomeFunction]: Some error message 
 *
 * Code:
 *    KSLOG_INFO(@"Info about %@", someObject);
 *
 * Prints:
 *    2011-09-25 05:44:05.239 TestApp[4473:f803] INFO : SomeClass.m (20): -[SomeFunction]: Info about <NSObject: 0xb622840>
 *
 *
 * The "BASIC" versions of the macros behave exactly like NSLog, except they
 * respect the KSLogger_Level setting:
 *
 * Code:
 *    KSLOGBASIC_ERROR(@"A basic log entry");
 *
 * Prints:
 *    2011-09-25 05:44:05.916 TestApp[4473:f803] A basic log entry
 *
 *
 * Note: The C versions use "" instead of @"" in the format field, and do not
 *       print the NSLog preamble:
 *
 * Objective-C version:
 *
 *    2011-09-25 05:41:01.379 TestApp[4439:f803] ERROR: SomeClass.m (21): -[SomeFunction]: Some error message
 *
 * C version:
 *    ERROR: SomeClass.m (21): -[SomeFunction]: Some error message
 *
 *
 * =============
 * LOCAL LOGGING
 * =============
 *
 * You can control logging messages at the local file level using the
 * "KSLogger_LocalLevel" define. Note that it must be defined BEFORE
 * including KSLoggerC.h or KSLoggerObjC.h
 *
 * The KSLOG_XX() and KSLOGBASIC_XX() macros will print out based on the LOWER
 * of KSLogger_Level and KSLogger_LocalLevel, so if KSLogger_Level is DEBUG
 * and KSLogger_LocalLevel is TRACE, it will print all the way down to the trace
 * level for that file, and to the debug level everywhere else.
 *
 * Example:
 *
 * // KSLogger_LocalLevel, if defined, MUST be defined BEFORE including KSLoggerXX.h
 * #define KSLogger_LocalLevel TRACE
 * #import "KSLoggerObjC.h"
 *
 *
 * ===============
 * IMPORTANT NOTES
 * ===============
 *
 * The C logger is async-safe, but is limited in how big of a log message it can
 * print. By default it is 1024 bytes, but the preprocessor define
 * KSLogger_CBufferSize can set the size (only during compilation of KSLogger).
 */

void i_kslog_c(const char* level,
               const char* file,
               unsigned int line,
               const char* function,
               const unsigned char* binary_data,
               int byte_count, 
               const char* fmt, ...);

void i_kslog_c_basic(const unsigned char* binary_data,
                     int byte_count,
                     const char* fmt, ...);

#define i_KSLOG_FULL i_kslog_c
#define i_KSLOG_BASIC i_kslog_c_basic


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


#ifndef KSLogger_Level
    #define KSLogger_Level INFO
#endif

#ifndef KSLogger_LocalLevel
    #define KSLogger_LocalLevel NONE
#endif

#define a_KSLOG_FULL(LEVEL, BINARY_DATA, BYTE_COUNT, FMT, ...) \
    i_KSLOG_FULL(LEVEL, \
                 __FILE__, \
                 __LINE__, \
                 __PRETTY_FUNCTION__, \
                 BINARY_DATA, \
                 BYTE_COUNT, \
                 FMT, \
                 ##__VA_ARGS__)



/** Log an error.
 * Normal version prints out full context. Basic version prints directly.
 *
 * @param FMT The format specifier, followed by its arguments.
 */
#if KSLogger_Level >= ERROR || KSLogger_LocalLevel >= ERROR
    #define KSLOG_DATA_ERROR(BINARY_DATA, BYTE_COUNT, FMT, ...) a_KSLOG_FULL("ERROR", BINARY_DATA, BYTE_COUNT, FMT, ##__VA_ARGS__)
    #define KSLOGBASIC_DATA_ERROR(BINARY_DATA, BYTE_COUNT, FMT, ...) i_KSLOG_BASIC(BINARY_DATA, BYTE_COUNT, FMT, ##__VA_ARGS__)
#else
    #define KSLOG_DATA_ERROR(BINARY_DATA, BYTE_COUNT, FMT, ...)
    #define KSLOGBASIC_DATA_ERROR(BINARY_DATA, BYTE_COUNT, FMT, ...)
#endif
#define KSLOG_ERROR(FMT, ...) KSLOG_DATA_ERROR(NULL, 0, FMT, ##__VA_ARGS__)
#define KSLOGBASIC_ERROR(FMT, ...) KSLOGBASIC_DATA_ERROR(NULL, 0, FMT, ##__VA_ARGS__)

/** Log a warning.
 * Normal version prints out full context. Basic version prints directly.
 *
 * @param FMT The format specifier, followed by its arguments.
 */
#if KSLogger_Level >= WARN || KSLogger_LocalLevel >= WARN
    #define KSLOG_DATA_WARN(BINARY_DATA, BYTE_COUNT, FMT, ...) a_KSLOG_FULL("WARN", BINARY_DATA, BYTE_COUNT, FMT, ##__VA_ARGS__)
    #define KSLOGBASIC_DATA_WARN(BINARY_DATA, BYTE_COUNT, FMT, ...) i_KSLOG_BASIC(BINARY_DATA, BYTE_COUNT, FMT, ##__VA_ARGS__)
#else
    #define KSLOG_DATA_WARN(BINARY_DATA, BYTE_COUNT, FMT, ...)
    #define KSLOGBASIC_DATA_WARN(BINARY_DATA, BYTE_COUNT, FMT, ...)
#endif
#define KSLOG_WARN(FMT, ...) KSLOG_DATA_WARN(NULL, 0, FMT, ##__VA_ARGS__)
#define KSLOGBASIC_WARN(FMT, ...) KSLOGBASIC_DATA_WARN(NULL, 0, FMT, ##__VA_ARGS__)

/** Log an info message.
 * Normal version prints out full context. Basic version prints directly.
 *
 * @param FMT The format specifier, followed by its arguments.
 */
#if KSLogger_Level >= INFO || KSLogger_LocalLevel >= INFO
    #define KSLOG_DATA_INFO(BINARY_DATA, BYTE_COUNT, FMT, ...) a_KSLOG_FULL("INFO", BINARY_DATA, BYTE_COUNT, FMT, ##__VA_ARGS__)
    #define KSLOGBASIC_DATA_INFO(BINARY_DATA, BYTE_COUNT, FMT, ...) i_KSLOG_BASIC(BINARY_DATA, BYTE_COUNT, FMT, ##__VA_ARGS__)
#else
    #define KSLOG_DATA_INFO(BINARY_DATA, BYTE_COUNT, FMT, ...)
    #define KSLOGBASIC_DATA_INFO(BINARY_DATA, BYTE_COUNT, FMT, ...)
#endif
#define KSLOG_INFO(FMT, ...) KSLOG_DATA_INFO(NULL, 0, FMT, ##__VA_ARGS__)
#define KSLOGBASIC_INFO(FMT, ...) KSLOGBASIC_DATA_INFO(NULL, 0, FMT, ##__VA_ARGS__)

/** Log a debug message.
 * Normal version prints out full context. Basic version prints directly.
 *
 * @param FMT The format specifier, followed by its arguments.
 */
#if KSLogger_Level >= DEBUG || KSLogger_LocalLevel >= DEBUG
    #define KSLOG_DATA_DEBUG(BINARY_DATA, BYTE_COUNT, FMT, ...) a_KSLOG_FULL("DEBUG", BINARY_DATA, BYTE_COUNT, FMT, ##__VA_ARGS__)
    #define KSLOGBASIC_DATA_DEBUG(BINARY_DATA, BYTE_COUNT, FMT, ...) i_KSLOG_BASIC(BINARY_DATA, BYTE_COUNT, FMT, ##__VA_ARGS__)
#else
    #define KSLOG_DATA_DEBUG(BINARY_DATA, BYTE_COUNT, FMT, ...)
    #define KSLOGBASIC_DATA_DEBUG(BINARY_DATA, BYTE_COUNT, FMT, ...)
#endif
#define KSLOG_DEBUG(FMT, ...) KSLOG_DATA_DEBUG(NULL, 0, FMT, ##__VA_ARGS__)
#define KSLOGBASIC_DEBUG(FMT, ...) KSLOGBASIC_DATA_DEBUG(NULL, 0, FMT, ##__VA_ARGS__)

/** Log a trace message.
 * Normal version prints out full context. Basic version prints directly.
 *
 * @param FMT The format specifier, followed by its arguments.
 */
#if KSLogger_Level >= TRACE || KSLogger_LocalLevel >= TRACE
    #define KSLOG_DATA_TRACE(BINARY_DATA, BYTE_COUNT, FMT, ...) a_KSLOG_FULL("TRACE", BINARY_DATA, BYTE_COUNT, FMT, ##__VA_ARGS__)
    #define KSLOGBASIC_DATA_TRACE(BINARY_DATA, BYTE_COUNT, FMT, ...) i_KSLOG_BASIC(BINARY_DATA, BYTE_COUNT, FMT, ##__VA_ARGS__)
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
