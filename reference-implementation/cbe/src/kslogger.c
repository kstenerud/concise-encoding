//
//  KSLoggerC.c
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


#include "kslogger.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


#ifndef KSLogger_CBufferSize
    #define KSLogger_CBufferSize 1024
#endif


static const char* lastPathEntry(const char* path)
{
    const char* lastFile = strrchr(path, '/');
    return lastFile == 0 ? path : lastFile + 1;
}

static void writeToStdout(const char* str)
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

static void write_hex(const unsigned char* ptr, const int length)
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

void i_kslog_c_basic(const unsigned char* binary_data,
                     int byte_count,
                     const char* fmt, ...)
{
    if(*fmt != 0)
    {
        char buffer[KSLogger_CBufferSize];
    
        va_list args;
        va_start(args,fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);
        
        writeToStdout(buffer);
        write_hex(binary_data, byte_count);
        write(STDOUT_FILENO, "\n", 1);
    }
}

void i_kslog_c(const char* level,
               const char* file,
               unsigned int line,
               const char* function,
               const unsigned char* binary_data,
               int byte_count, 
               const char* fmt, ...)
{
    if(*fmt != 0)
    {
        char buffer[KSLogger_CBufferSize];

        snprintf(buffer, sizeof(buffer), "%s: %s (%u): %s: ",
                 level, lastPathEntry(file), line, function);
        writeToStdout(buffer);

        va_list args;
        va_start(args,fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);
        
        writeToStdout(buffer);
        write_hex(binary_data, byte_count);
        write(STDOUT_FILENO, "\n", 1);
    }
}
