#include "cbe_internal.h"
#include "version.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"

const char* cbe_version()
{
    return PROJECT_VERSION;
}

bool cbe_verify_comment_data(const char* const start,
                             const int64_t byte_count)
{
    KSLOG_DEBUG("start %p, byte_count %d", start, byte_count);
    const char* ptr = start;
    const char* const end = ptr + byte_count;
    while(ptr < end)
    {
        char ch = *ptr++;
        if((ch < 0x20 && ch != 0x09) || ch == 0x7f)
        {
            return false;
        }
        // TODO: Check for u+0085 (next line)
        // TODO: Check for u+2028 (line separator)
        // TODO: Check for u+2029 (paragraph separator)
    }
    return true;
}
