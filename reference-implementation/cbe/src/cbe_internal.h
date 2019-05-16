#ifndef cbe_internal_H
#define cbe_internal_H

#include "cbe/cbe.h"

typedef enum
{
    TYPE_SMALLINT_MIN      = -106,
    TYPE_SMALLINT_MAX      = 105,
    TYPE_POS_INT_8         = 0x6a,
    TYPE_POS_INT_16        = 0x6b,
    TYPE_POS_INT_32        = 0x6c,
    TYPE_POS_INT_64        = 0x6d,
    TYPE_POS_INT_128       = 0x6e,
    TYPE_NIL               = 0x6f,
    TYPE_FALSE             = 0x70,
    TYPE_TRUE              = 0x71,
    TYPE_FLOAT_32          = 0x72,
    TYPE_FLOAT_64          = 0x73,
    TYPE_FLOAT_128         = 0x74,
    TYPE_DECIMAL_32        = 0x75,
    TYPE_DECIMAL_64        = 0x76,
    TYPE_DECIMAL_128       = 0x77,
    TYPE_SMALLTIME         = 0x78,
    TYPE_NANOTIME          = 0x79,
    TYPE_NEG_INT_8         = 0x7a,
    TYPE_NEG_INT_16        = 0x7b,
    TYPE_NEG_INT_32        = 0x7c,
    TYPE_NEG_INT_64        = 0x7d,
    TYPE_NEG_INT_128       = 0x7e,
    TYPE_PADDING           = 0x7f,
    TYPE_STRING_0          = 0x80,
    TYPE_STRING_1          = 0x81,
    TYPE_STRING_2          = 0x82,
    TYPE_STRING_3          = 0x83,
    TYPE_STRING_4          = 0x84,
    TYPE_STRING_5          = 0x85,
    TYPE_STRING_6          = 0x86,
    TYPE_STRING_7          = 0x87,
    TYPE_STRING_8          = 0x88,
    TYPE_STRING_9          = 0x89,
    TYPE_STRING_10         = 0x8a,
    TYPE_STRING_11         = 0x8b,
    TYPE_STRING_12         = 0x8c,
    TYPE_STRING_13         = 0x8d,
    TYPE_STRING_14         = 0x8e,
    TYPE_STRING_15         = 0x8f,
    TYPE_STRING            = 0x90,
    TYPE_BYTES             = 0x91,
    TYPE_COMMENT           = 0x92,
    TYPE_LIST              = 0x93,
    TYPE_MAP               = 0x94,
    TYPE_END_CONTAINER     = 0x95,
} cbe_type_field;

typedef enum
{
    ARRAY_TYPE_STRING,
    ARRAY_TYPE_BYTES,
    ARRAY_TYPE_COMMENT,
} array_type;



// Force the compiler to generate handler code for unaligned accesses.
#define DEFINE_SAFE_STRUCT(NAME, TYPE) typedef struct __attribute__((__packed__)) {TYPE contents;} NAME
DEFINE_SAFE_STRUCT(safe_uint_16,     uint16_t);
DEFINE_SAFE_STRUCT(safe_uint_32,     uint32_t);
DEFINE_SAFE_STRUCT(safe_uint_64,     uint64_t);
DEFINE_SAFE_STRUCT(safe_uint_128,    uint128_ct);
DEFINE_SAFE_STRUCT(safe_int_16,      int16_t);
DEFINE_SAFE_STRUCT(safe_int_32,      int32_t);
DEFINE_SAFE_STRUCT(safe_int_64,      int64_t);
DEFINE_SAFE_STRUCT(safe_int_128,     int128_ct);
DEFINE_SAFE_STRUCT(safe_float_32,    float);
DEFINE_SAFE_STRUCT(safe_float_64,    double);
DEFINE_SAFE_STRUCT(safe_float_128,   float128_ct);
DEFINE_SAFE_STRUCT(safe_decimal_32,  dec32_ct);
DEFINE_SAFE_STRUCT(safe_decimal_64,  dec64_ct);
DEFINE_SAFE_STRUCT(safe_decimal_128, dec128_ct);
DEFINE_SAFE_STRUCT(safe_smalltime,   smalltime);
DEFINE_SAFE_STRUCT(safe_nanotime,    nanotime);



static inline int get_max_container_depth_or_default(int max_container_depth)
{
    return max_container_depth > 0 ? max_container_depth : CBE_DEFAULT_MAX_CONTAINER_DEPTH;
}

static inline void zero_memory(void* const memory, const int byte_count)
{
    uint8_t* ptr = memory;
    uint8_t* const end = ptr + byte_count;
    while(ptr < end)
    {
        *ptr++ = 0;
    }
}

bool cbe_validate_string(const uint8_t* const start, const int64_t byte_count);

bool cbe_validate_comment(const uint8_t* const start, const int64_t byte_count);

#endif // cbe_internal_H
