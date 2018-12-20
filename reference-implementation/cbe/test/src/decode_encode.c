#include <stdlib.h>
#include <cbe/cbe.h>
#include <memory.h>
#include "decode_encode.h"
#include "old_test_helpers.h"
#include "kslogger.h"

#define RETURN_WITH_WARN_ON_ERROR(...) \
{ \
    cbe_encode_status status = (__VA_ARGS__); \
    if(status != CBE_ENCODE_STATUS_OK) \
    { \
        KSLOG_WARN("Code [" #__VA_ARGS__ "] returned encode status %d", status); \
        return false; \
    } \
    return true; \
}

#define DEFINE_DECODE_ENCODE_CALLBACK_0_PARAMETER(NAME_FRAGMENT) \
static bool on_ ## NAME_FRAGMENT(struct cbe_decode_process* decode_process) \
{ \
    struct cbe_encode_process* encode_process = (struct cbe_encode_process*)cbe_decode_get_user_context(decode_process); \
    RETURN_WITH_WARN_ON_ERROR(cbe_encode_ ## NAME_FRAGMENT(encode_process)); \
}
#define DEFINE_DECODE_ENCODE_CALLBACK_1_PARAMETER(NAME_FRAGMENT, TYPE) \
static bool on_ ## NAME_FRAGMENT(struct cbe_decode_process* decode_process, TYPE value) \
{ \
    struct cbe_encode_process* encode_process = (struct cbe_encode_process*)cbe_decode_get_user_context(decode_process); \
    RETURN_WITH_WARN_ON_ERROR(cbe_encode_ ## NAME_FRAGMENT(encode_process, value)); \
}
#define DEFINE_DECODE_ENCODE_CALLBACK_2_PARAMETER(NAME_FRAGMENT, TYPE1, TYPE2) \
static bool on_ ## NAME_FRAGMENT(struct cbe_decode_process* decode_process, TYPE1 param1, TYPE2 param2) \
{ \
    struct cbe_encode_process* encode_process = (struct cbe_encode_process*)cbe_decode_get_user_context(decode_process); \
    RETURN_WITH_WARN_ON_ERROR(cbe_encode_ ## NAME_FRAGMENT(encode_process, param1, param2)); \
}
DEFINE_DECODE_ENCODE_CALLBACK_1_PARAMETER(add_boolean, bool)
DEFINE_DECODE_ENCODE_CALLBACK_1_PARAMETER(add_int_8, int8_t)
DEFINE_DECODE_ENCODE_CALLBACK_1_PARAMETER(add_int_16, int16_t)
DEFINE_DECODE_ENCODE_CALLBACK_1_PARAMETER(add_int_32, int32_t)
DEFINE_DECODE_ENCODE_CALLBACK_1_PARAMETER(add_int_64, int64_t)
DEFINE_DECODE_ENCODE_CALLBACK_1_PARAMETER(add_int_128, __int128)
DEFINE_DECODE_ENCODE_CALLBACK_1_PARAMETER(add_float_32, float)
DEFINE_DECODE_ENCODE_CALLBACK_1_PARAMETER(add_float_64, double)
DEFINE_DECODE_ENCODE_CALLBACK_1_PARAMETER(add_float_128, __float128)
DEFINE_DECODE_ENCODE_CALLBACK_1_PARAMETER(add_decimal_32, _Decimal32)
DEFINE_DECODE_ENCODE_CALLBACK_1_PARAMETER(add_decimal_64, _Decimal64)
DEFINE_DECODE_ENCODE_CALLBACK_1_PARAMETER(add_decimal_128, _Decimal128)
DEFINE_DECODE_ENCODE_CALLBACK_1_PARAMETER(add_time, smalltime)
DEFINE_DECODE_ENCODE_CALLBACK_0_PARAMETER(add_empty)
DEFINE_DECODE_ENCODE_CALLBACK_0_PARAMETER(end_container)
DEFINE_DECODE_ENCODE_CALLBACK_0_PARAMETER(begin_list)
DEFINE_DECODE_ENCODE_CALLBACK_0_PARAMETER(begin_map)
DEFINE_DECODE_ENCODE_CALLBACK_1_PARAMETER(begin_string, int64_t)
DEFINE_DECODE_ENCODE_CALLBACK_1_PARAMETER(begin_binary, int64_t)
DEFINE_DECODE_ENCODE_CALLBACK_2_PARAMETER(add_data, const uint8_t*, int64_t)

struct cbe_decode_process* new_decode_encode_process(cbe_decode_callbacks* callbacks, struct cbe_encode_process* encode_process)
{
    FILL_CALLBACKS(callbacks);
    return cbe_decode_begin(callbacks, encode_process);
}

cbe_decode_status perform_decode_encode(const uint8_t* document, int byte_count, struct cbe_encode_process* encode_process)
{
    cbe_decode_callbacks callbacks;
    struct cbe_decode_process* decode_process = new_decode_encode_process(&callbacks, encode_process);
    cbe_decode_status status_feed = cbe_decode_feed(decode_process, document, byte_count);
    cbe_decode_status status_end = cbe_decode_end(decode_process);
    return status_feed != CBE_DECODE_STATUS_OK ? status_feed : status_end;
}
