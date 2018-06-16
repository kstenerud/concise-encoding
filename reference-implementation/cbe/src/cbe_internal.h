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
    TYPE_EMPTY             = 0x68,
    TYPE_DATE_40           = 0x69,
    TYPE_DATE_48           = 0x6a,
    TYPE_DATE_64           = 0x6b,
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
    TYPE_ARRAY_DATE        = 0x8c,
    TYPE_INT_16            = 0x8d,
    TYPE_INT_32            = 0x8e,
    TYPE_INT_64            = 0x8f,
    TYPE_INT_128           = 0x90,
    TYPE_FLOAT_32          = 0x91,
    TYPE_FLOAT_64          = 0x92,
    TYPE_FLOAT_128         = 0x93,
    TYPE_DECIMAL_64        = 0x94,
    TYPE_DECIMAL_128       = 0x95,
    TYPE_FALSE             = 0x96,
    TYPE_TRUE              = 0x97,
} type_field;


typedef enum
{
    LENGTH_FIELD_WIDTH_6_BIT  = 0,
    LENGTH_FIELD_WIDTH_14_BIT = 1,
    LENGTH_FIELD_WIDTH_30_BIT = 2,
    LENGTH_FIELD_WIDTH_62_BIT = 3,
} length_field_width;

#define MAX_VALUE_6_BIT  0x3f
#define MAX_VALUE_14_BIT 0x3fff
#define MAX_VALUE_30_BIT 0x3fffffff

#define DATE_MODULO_MICROSECOND     1000UL
#define DATE_MODULO_MILLISECOND     1000UL
#define DATE_MODULO_SECOND            61UL
#define DATE_MODULO_MINUTE            60UL
#define DATE_MODULO_HOUR              24UL
#define DATE_MODULO_DAY               31UL
#define DATE_MODULO_MONTH             12UL

#define DATE_MULTIPLIER_MICROSECOND 1UL
#define DATE_MULTIPLIER_MILLISECOND ((DATE_MULTIPLIER_MICROSECOND) * DATE_MODULO_MICROSECOND)
#define DATE_MULTIPLIER_SECOND      ((DATE_MULTIPLIER_MILLISECOND) * DATE_MODULO_MILLISECOND)
#define DATE_MULTIPLIER_MINUTE      ((DATE_MULTIPLIER_SECOND) * DATE_MODULO_SECOND)
#define DATE_MULTIPLIER_HOUR        ((DATE_MULTIPLIER_MINUTE) * DATE_MODULO_MINUTE)
#define DATE_MULTIPLIER_DAY         ((DATE_MULTIPLIER_HOUR)   * DATE_MODULO_HOUR)
#define DATE_MULTIPLIER_MONTH       ((DATE_MULTIPLIER_DAY)    * DATE_MODULO_DAY)
#define DATE_MULTIPLIER_YEAR        ((DATE_MULTIPLIER_MONTH)  * DATE_MODULO_MONTH)

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
DEFINE_SAFE_STRUCT(safe_float_128,   long double);
#if CBE_HAS_DECIMAL_SUPPORT
DEFINE_SAFE_STRUCT(safe_decimal_64,  _Decimal64);
DEFINE_SAFE_STRUCT(safe_decimal_128, _Decimal128);
#endif

#ifdef __cplusplus 
}
#endif
#endif // cbe_internal_H
