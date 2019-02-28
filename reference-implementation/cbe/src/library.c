#include "cbe_internal.h"
#include "version.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"

const char* cbe_version()
{
    return PROJECT_VERSION;
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

bool cbe_validate_string(const uint8_t* const start, const int64_t byte_count)
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

bool cbe_validate_comment(const uint8_t* const start, const int64_t byte_count)
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
