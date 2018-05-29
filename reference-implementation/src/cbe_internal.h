#ifndef cbe_internal_H
#define cbe_internal_H
#ifdef __cplusplus 
extern "C" {
#endif


typedef enum
{
	SMALLINT_MIN         = -100,
	SMALLINT_MAX         = 100,
	TYPE_EMPTY           = 0x65,
	TYPE_FALSE           = 0x66,
	TYPE_TRUE            = 0x67,
	TYPE_DATE            = 0x68,
	TYPE_TIMESTAMP_MS    = 0x69,
	TYPE_TIMESTAMP_NS    = 0x6a,
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
	TYPE_DECIMAL_BID_64  = 0x98,
	TYPE_DECIMAL_BID_128 = 0x99,
	TYPE_DECIMAL_DPD_64  = 0x9a,
	TYPE_DECIMAL_DPD_128 = 0x9b,
} type_field;


typedef enum
{
	LENGTH_8BIT_MIN = 0x00,
	LENGTH_8BIT_MAX = 0xfc,
	LENGTH_64BIT    = 0xfd,
	LENGTH_32BIT    = 0xfe,
	LENGTH_16BIT    = 0xff,
} length_field;

#ifdef __cplusplus 
}
#endif
#endif // cbe_internal_H
