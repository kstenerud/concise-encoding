#ifndef cbe_internal_H
#define cbe_internal_H
#ifdef __cplusplus 
extern "C" {
#endif

#include "cbe/cbe.h"

typedef enum
{
    TYPE_SMALLINT_MIN      = -104,
    TYPE_SMALLINT_MAX      = 103,
    TYPE_FALSE             = 0x68,
    TYPE_TRUE              = 0x69,
    TYPE_TIME_40           = 0x6a,
    TYPE_TIME_64           = 0x6b,
    TYPE_LIST              = 0x6c,
    TYPE_MAP               = 0x6d,
    TYPE_END_CONTAINER     = 0x6e,
    TYPE_PADDING           = 0x6f,
    TYPE_STRING_0          = 0x70,
    TYPE_STRING_1          = 0x71,
    TYPE_STRING_2          = 0x72,
    TYPE_STRING_3          = 0x73,
    TYPE_STRING_4          = 0x74,
    TYPE_STRING_5          = 0x75,
    TYPE_STRING_6          = 0x76,
    TYPE_STRING_7          = 0x77,
    TYPE_STRING_8          = 0x78,
    TYPE_STRING_9          = 0x79,
    TYPE_STRING_10         = 0x7a,
    TYPE_STRING_11         = 0x7b,
    TYPE_STRING_12         = 0x7c,
    TYPE_STRING_13         = 0x7d,
    TYPE_STRING_14         = 0x7e,
    TYPE_STRING_MAX        = 0x7f,
    TYPE_ARRAY_STRING      = 0x80,
    TYPE_ARRAY_BOOLEAN     = 0x81,
    TYPE_ARRAY_INT_8       = 0x82,
    TYPE_ARRAY_INT_16      = 0x83,
    TYPE_ARRAY_INT_32      = 0x84,
    TYPE_ARRAY_INT_64      = 0x85,
    TYPE_ARRAY_INT_128     = 0x86,
    TYPE_ARRAY_FLOAT_32    = 0x87,
    TYPE_ARRAY_FLOAT_64    = 0x88,
    TYPE_ARRAY_FLOAT_128   = 0x89,
    TYPE_ARRAY_DECIMAL_64  = 0x8a,
    TYPE_ARRAY_DECIMAL_128 = 0x8b,
    TYPE_ARRAY_TIME_40     = 0x8c,
    TYPE_ARRAY_TIME_64     = 0x8d,
    TYPE_INT_16            = 0x8e,
    TYPE_INT_32            = 0x8f,
    TYPE_INT_64            = 0x90,
    TYPE_INT_128           = 0x91,
    TYPE_FLOAT_32          = 0x92,
    TYPE_FLOAT_64          = 0x93,
    TYPE_FLOAT_128         = 0x94,
    TYPE_DECIMAL_64        = 0x95,
    TYPE_DECIMAL_128       = 0x96,
    TYPE_EMPTY             = 0x97,
} cbe_type_field;

typedef uint8_t cbe_encoded_type_field;

typedef enum
{
    LENGTH_FIELD_WIDTH_6_BIT  = 0,
    LENGTH_FIELD_WIDTH_14_BIT = 1,
    LENGTH_FIELD_WIDTH_30_BIT = 2,
    LENGTH_FIELD_WIDTH_62_BIT = 3,
} cbe_length_field_width;

typedef enum
{
    MAX_VALUE_6_BIT  = 0x3f,
    MAX_VALUE_14_BIT = 0x3fff,
    MAX_VALUE_30_BIT = 0x3fffffff,
} cbe_max_value;

typedef enum
{
    TIME_40_BITSHIFT_YEAR   = 26,
    TIME_40_BITSHIFT_MONTH  = 22,
    TIME_40_BITSHIFT_DAY    = 17,
    TIME_40_BITSHIFT_HOUR   = 12,
    TIME_40_BITSHIFT_MINUTE =  6,
} cbe_time_40_bitshift;

typedef enum
{
    TIME_64_BITSHIFT_YEAR   = 46,
    TIME_64_BITSHIFT_MONTH  = 42,
    TIME_64_BITSHIFT_DAY    = 37,
    TIME_64_BITSHIFT_HOUR   = 32,
    TIME_64_BITSHIFT_MINUTE = 26,
    TIME_64_BITSHIFT_SECOND = 20,
} cbe_time_64_bitshift;

typedef enum
{
    TIME_MASK_MONTH       = 0x0f,
    TIME_MASK_DAY         = 0x1f,
    TIME_MASK_HOUR        = 0x1f,
    TIME_MASK_MINUTE      = 0x3f,
    TIME_MASK_SECOND      = 0x3f,
    TIME_MASK_MICROSECOND = 0xfffff,
} cbe_time_mask;


// Force the compiler to generate handler code for unaligned accesses.
#define DEFINE_SAFE_STRUCT(NAME, TYPE) typedef struct __attribute__((__packed__)) {TYPE contents;} NAME
DEFINE_SAFE_STRUCT(safe_uint_16,     uint16_t);
DEFINE_SAFE_STRUCT(safe_uint_32,     uint32_t);
DEFINE_SAFE_STRUCT(safe_uint_64,     uint64_t);
DEFINE_SAFE_STRUCT(safe_int_16,      int16_t);
DEFINE_SAFE_STRUCT(safe_int_32,      int32_t);
DEFINE_SAFE_STRUCT(safe_int_64,      int64_t);
DEFINE_SAFE_STRUCT(safe_int_128,     __int128);
DEFINE_SAFE_STRUCT(safe_float_32,    float);
DEFINE_SAFE_STRUCT(safe_float_64,    double);
DEFINE_SAFE_STRUCT(safe_float_128,   __float128);
#if CBE_HAS_DECIMAL_SUPPORT
DEFINE_SAFE_STRUCT(safe_decimal_64,  _Decimal64);
DEFINE_SAFE_STRUCT(safe_decimal_128, _Decimal128);
#endif
DEFINE_SAFE_STRUCT(safe_time_64,     uint64_t);

#ifdef __cplusplus 
}
#endif
#endif // cbe_internal_H
