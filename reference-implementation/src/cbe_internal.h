#ifndef cbe_internal_H
#define cbe_internal_H
#ifdef __cplusplus 
extern "C" {
#endif

#include "cbe/cbe_config.h"

typedef enum
{
	TYPE_SMALLINT_MIN    = -101,
	TYPE_SMALLINT_MAX    = 101,
	TYPE_FALSE           = 0x66,
	TYPE_TRUE            = 0x67,
	TYPE_DATE            = 0x68,
	TYPE_TIMESTAMP_MILLI = 0x69,
	TYPE_TIMESTAMP_MICRO = 0x6a,
	TYPE_END_CONTAINER   = 0x6b,
	TYPE_LIST            = 0x6c,
	TYPE_MAP             = 0x6d,
	TYPE_ARRAY           = 0x6e,
	TYPE_STRING          = 0x6f,
	TYPE_STRING_0        = 0x70,
	TYPE_STRING_1        = 0x71,
	TYPE_STRING_2        = 0x72,
	TYPE_STRING_3        = 0x73,
	TYPE_STRING_4        = 0x74,
	TYPE_STRING_5        = 0x75,
	TYPE_STRING_6        = 0x76,
	TYPE_STRING_7        = 0x77,
	TYPE_STRING_8        = 0x78,
	TYPE_STRING_9        = 0x79,
	TYPE_STRING_10       = 0x7a,
	TYPE_STRING_11       = 0x7b,
	TYPE_STRING_12       = 0x7c,
	TYPE_STRING_13       = 0x7d,
	TYPE_STRING_14       = 0x7e,
	TYPE_STRING_15       = 0x7f,
	TYPE_BYTES_0         = 0x80,
	TYPE_BYTES_1         = 0x81,
	TYPE_BYTES_2         = 0x82,
	TYPE_BYTES_3         = 0x83,
	TYPE_BYTES_4         = 0x84,
	TYPE_BYTES_5         = 0x85,
	TYPE_BYTES_6         = 0x86,
	TYPE_BYTES_7         = 0x87,
	TYPE_BYTES_8         = 0x88,
	TYPE_BYTES_9         = 0x89,
	TYPE_BYTES_10        = 0x8a,
	TYPE_BYTES_11        = 0x8b,
	TYPE_BYTES_12        = 0x8c,
	TYPE_BYTES_13        = 0x8d,
	TYPE_BYTES_14        = 0x8e,
	TYPE_BYTES_15        = 0x8f,
	TYPE_BYTES           = 0x90,
	TYPE_INT_16          = 0x91,
	TYPE_INT_32          = 0x92,
	TYPE_INT_64          = 0x93,
	TYPE_INT_128         = 0x94,
	TYPE_FLOAT_32        = 0x95,
	TYPE_FLOAT_64        = 0x96,
	TYPE_FLOAT_128       = 0x97,
	TYPE_DECIMAL_64      = 0x98,
	TYPE_DECIMAL_128     = 0x99,
	TYPE_EMPTY           = 0x9a,
} type_field;

typedef enum
{
	BYTES_LENGTH_8_BIT_MAX = 0xfc,
	BYTES_LENGTH_16_BIT    = 0xfd,
	BYTES_LENGTH_32_BIT    = 0xfe,
	BYTES_LENGTH_64_BIT    = 0xff,
} bytes_length;

typedef enum
{
	ARRAY_CONTENT_TYPE_BOOLEAN      = 0x00,
	ARRAY_CONTENT_TYPE_INT_16       = 0x10,
	ARRAY_CONTENT_TYPE_INT_32       = 0x20,
	ARRAY_CONTENT_TYPE_INT_64       = 0x30,
	ARRAY_CONTENT_TYPE_INT_128      = 0x40,
	ARRAY_CONTENT_TYPE_FLOAT_32     = 0x50,
	ARRAY_CONTENT_TYPE_FLOAT_64     = 0x60,
	ARRAY_CONTENT_TYPE_FLOAT_128    = 0x70,
	ARRAY_CONTENT_TYPE_DECIMAL_64   = 0x80,
	ARRAY_CONTENT_TYPE_DECIMAL_128  = 0x90,
	ARRAY_CONTENT_TYPE_DATE         = 0xa0,
	ARRAY_CONTENT_TYPE_TIMESTAMP_MS = 0xb0,
	ARRAY_CONTENT_TYPE_TIMESTAMP_NS = 0xc0,
} array_content_type_field;

typedef enum
{
	ARRAY_LENGTH_SMALL_MAX = 0x0b,
	ARRAY_LENGTH_8_BIT     = 0x0c,
	ARRAY_LENGTH_16_BIT    = 0x0d,
	ARRAY_LENGTH_32_BIT    = 0x0e,
	ARRAY_LENGTH_64_BIT    = 0x0f,
} array_length_field;

#define DATE_MODULO_MICROSECOND 1000000UL
#define DATE_MODULO_MILLISECOND    1000UL
#define DATE_MODULO_SECOND           61UL
#define DATE_MODULO_MINUTE           60UL
#define DATE_MODULO_HOUR             24UL
#define DATE_MODULO_DAY              31UL
#define DATE_MODULO_MONTH            12UL

#define DATE_MULTIPLIER_SECOND       1UL
#define DATE_MULTIPLIER_MINUTE       ((DATE_MULTIPLIER_SECOND) * DATE_MODULO_SECOND)
#define DATE_MULTIPLIER_HOUR         ((DATE_MULTIPLIER_MINUTE) * DATE_MODULO_MINUTE)
#define DATE_MULTIPLIER_DAY          ((DATE_MULTIPLIER_HOUR)   * DATE_MODULO_HOUR)
#define DATE_MULTIPLIER_MONTH        ((DATE_MULTIPLIER_DAY)    * DATE_MODULO_DAY)
#define DATE_MULTIPLIER_YEAR         ((DATE_MULTIPLIER_MONTH)  * DATE_MODULO_MONTH)

#define TS_MS_MULTIPLIER_MILLISECOND 1UL
#define TS_MS_MULTIPLIER_SECOND      ((DATE_MULTIPLIER_SECOND) * DATE_MODULO_MILLISECOND)
#define TS_MS_MULTIPLIER_MINUTE      ((DATE_MULTIPLIER_MINUTE) * DATE_MODULO_MILLISECOND)
#define TS_MS_MULTIPLIER_HOUR        ((DATE_MULTIPLIER_HOUR)   * DATE_MODULO_MILLISECOND)
#define TS_MS_MULTIPLIER_DAY         ((DATE_MULTIPLIER_DAY)    * DATE_MODULO_MILLISECOND)
#define TS_MS_MULTIPLIER_MONTH       ((DATE_MULTIPLIER_MONTH)  * DATE_MODULO_MILLISECOND)
#define TS_MS_MULTIPLIER_YEAR        ((DATE_MULTIPLIER_YEAR)   * DATE_MODULO_MILLISECOND)

#define TS_US_MULTIPLIER_MICROSECOND 1UL
#define TS_US_MULTIPLIER_SECOND      ((DATE_MULTIPLIER_SECOND) * DATE_MODULO_MICROSECOND)
#define TS_US_MULTIPLIER_MINUTE      ((DATE_MULTIPLIER_MINUTE) * DATE_MODULO_MICROSECOND)
#define TS_US_MULTIPLIER_HOUR        ((DATE_MULTIPLIER_HOUR)   * DATE_MODULO_MICROSECOND)
#define TS_US_MULTIPLIER_DAY         ((DATE_MULTIPLIER_DAY)    * DATE_MODULO_MICROSECOND)
#define TS_US_MULTIPLIER_MONTH       ((DATE_MULTIPLIER_MONTH)  * DATE_MODULO_MICROSECOND)
#define TS_US_MULTIPLIER_YEAR        ((DATE_MULTIPLIER_YEAR)   * DATE_MODULO_MICROSECOND)

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
