#ifndef cbe_internal_H
#define cbe_internal_H
#ifdef __cplusplus 
extern "C" {
#endif

#include "cbe/cbe.h"


typedef enum
{
    TYPE_SMALLINT_MIN      = -110,
    TYPE_SMALLINT_MAX      = 109,
    TYPE_INT_16            = 0x6e,
    TYPE_INT_32            = 0x6f,
    TYPE_INT_64            = 0x70,
    TYPE_INT_128           = 0x71,
    TYPE_FLOAT_32          = 0x72,
    TYPE_FLOAT_64          = 0x73,
    TYPE_FLOAT_128         = 0x74,
    TYPE_DECIMAL_32        = 0x75,
    TYPE_DECIMAL_64        = 0x76,
    TYPE_DECIMAL_128       = 0x77,
    TYPE_TIME              = 0x78,
    TYPE_TRUE              = 0x79,
    TYPE_FALSE             = 0x7a,
    TYPE_LIST              = 0x7b,
    TYPE_MAP               = 0x7c,
    TYPE_END_CONTAINER     = 0x7d,
    TYPE_EMPTY             = 0x7e,
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
    TYPE_BINARY_DATA       = 0x91,
} cbe_type_field;

typedef uint8_t cbe_encoded_type_field;

typedef enum
{
    ARRAY_LENGTH_FIELD_WIDTH_6_BIT  = 0,
    ARRAY_LENGTH_FIELD_WIDTH_14_BIT = 1,
    ARRAY_LENGTH_FIELD_WIDTH_30_BIT = 2,
    ARRAY_LENGTH_FIELD_WIDTH_62_BIT = 3,
} cbe_array_length_field_width;

typedef enum
{
    MAX_VALUE_6_BIT  = 0x3f,
    MAX_VALUE_14_BIT = 0x3fff,
    MAX_VALUE_30_BIT = 0x3fffffff,
} cbe_max_value;


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
DEFINE_SAFE_STRUCT(safe_decimal_32,  _Decimal32);
DEFINE_SAFE_STRUCT(safe_decimal_64,  _Decimal64);
DEFINE_SAFE_STRUCT(safe_decimal_128, _Decimal128);
DEFINE_SAFE_STRUCT(safe_time,        smalltime);

#ifdef __cplusplus 
}
#endif
#endif // cbe_internal_H
