#ifndef cbe_encoder_H
#define cbe_encoder_H
#ifdef __cplusplus 
extern "C" {
#endif
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>


#ifdef __cplusplus
// C++ doesn't seem to have a POD implementation of decimal types.
	#define CBE_HAS_DECIMAL_SUPPORT 0
#else
	#define CBE_HAS_DECIMAL_SUPPORT 1
#endif


// Encode

typedef struct
{
	const uint8_t* const start;
	const uint8_t* const end;
	uint8_t* pos;
} cbe_buffer;

typedef struct
{
	unsigned int year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	unsigned int microsecond;
} cbe_date;


const char* cbe_version();

cbe_buffer cbe_new_buffer(uint8_t* const memory_start, uint8_t* const memory_end);

bool cbe_add_empty(cbe_buffer* const buffer);
bool cbe_add_boolean(cbe_buffer* const buffer, const bool value);
bool cbe_add_int(cbe_buffer* const buffer, const int value);
bool cbe_add_int_8(cbe_buffer* const buffer, const int8_t value);
bool cbe_add_int_16(cbe_buffer* const buffer, const int16_t value);
bool cbe_add_int_32(cbe_buffer* const buffer, const int32_t value);
bool cbe_add_int_64(cbe_buffer* const buffer, const int64_t value);
bool cbe_add_int_128(cbe_buffer* const buffer, const __int128 value);
bool cbe_add_float_32(cbe_buffer* const buffer, const float value);
bool cbe_add_float_64(cbe_buffer* const buffer, const double value);
bool cbe_add_float_128(cbe_buffer* const buffer, const long double value);
bool cbe_add_date(cbe_buffer* const buffer, const cbe_date* const date);
bool cbe_add_string(cbe_buffer* const buffer, const char* const str, const int byte_count);
bool cbe_start_list(cbe_buffer* const buffer);
bool cbe_start_map(cbe_buffer* const buffer);
bool cbe_end_container(cbe_buffer* const buffer);
bool cbe_add_bitfield(cbe_buffer* const buffer, const uint8_t* const packed_values, const int entity_count);
bool cbe_add_array_boolean(cbe_buffer* const buffer, const bool* const values, const int entity_count);
bool cbe_add_array_int_8(cbe_buffer* const buffer, const int8_t* const values, const int entity_count);
bool cbe_add_array_int_16(cbe_buffer* const buffer, const int16_t* const values, const int entity_count);
bool cbe_add_array_int_32(cbe_buffer* const buffer, const int32_t* const values, const int entity_count);
bool cbe_add_array_int_64(cbe_buffer* const buffer, const int64_t* const values, const int entity_count);
bool cbe_add_array_int_128(cbe_buffer* const buffer, const __int128* const values, const int entity_count);
bool cbe_add_array_float_32(cbe_buffer* const buffer, const float* const values, const int entity_count);
bool cbe_add_array_float_64(cbe_buffer* const buffer, const double* const values, const int entity_count);
bool cbe_add_array_float_128(cbe_buffer* const buffer, const long double* const values, const int entity_count);

#if CBE_HAS_DECIMAL_SUPPORT
bool cbe_add_decimal_64(cbe_buffer* const buffer, const _Decimal64 value);
bool cbe_add_decimal_128(cbe_buffer* const buffer, const _Decimal128 value);
bool cbe_add_array_decimal_64(cbe_buffer* const buffer, const _Decimal64* const values, const int entity_count);
bool cbe_add_array_decimal_128(cbe_buffer* const buffer, const _Decimal128* const values, const int entity_count);
#endif


// Decode


typedef enum
{
	CBE_NUMERIC_INT_8,
	CBE_NUMERIC_INT_16,
	CBE_NUMERIC_INT_32,
	CBE_NUMERIC_INT_64,
	CBE_NUMERIC_INT_128,
	CBE_NUMERIC_FLOAT_32,
	CBE_NUMERIC_FLOAT_64,
	CBE_NUMERIC_FLOAT_128,
	CBE_NUMERIC_DECIMAL_64,
	CBE_NUMERIC_DECIMAL_128,
} cbe_numeric_type;

typedef struct
{
	union
	{
		int8_t int_8;
		int16_t int_16;
		int32_t int_32;
		int64_t int_64;
		__int128 int_128;
		float float_32;
		double float_64;
		long double float_128;
#if CBE_HAS_DECIMAL_SUPPORT
		_Decimal64 decimal_64;
		_Decimal128 decimal_128;
#endif
	} data;
	cbe_numeric_type type;
} cbe_number;

typedef struct {
	bool (*on_error)(const char* message);
	bool (*on_empty)(void);
	bool (*on_bool)(bool value);
	bool (*on_number)(cbe_number value);
	bool (*on_date)(cbe_date* value);
	bool (*on_end_container)(void);
	bool (*on_list_start)(void);
	bool (*on_map_start)(void);
	bool (*on_bitfield)(const uint8_t* start, const uint64_t length);
	bool (*on_string)(const char* start, const char* end);
	bool (*on_array_int_8)(const int8_t* start, const int8_t* end);
	bool (*on_array_int_16)(const int16_t* start, const int16_t* end);
	bool (*on_array_int_32)(const int32_t* start, const int32_t* end);
	bool (*on_array_int_64)(const int64_t* start, const int64_t* end);
	bool (*on_array_int_128)(const __int128* start, const __int128* end);
	bool (*on_array_float_32)(const float* start, const float* end);
	bool (*on_array_float_64)(const double* start, const double* end);
	bool (*on_array_float_128)(const long double* start, const long double* end);
#if CBE_HAS_DECIMAL_SUPPORT
	bool (*on_array_decimal_64)(const _Decimal64* start, const _Decimal64* end);
	bool (*on_array_decimal_128)(const _Decimal128* start, const _Decimal128* end);
#endif
	bool (*on_array_date)(const uint64_t* start, const uint64_t* end);
} cbe_decode_callbacks;


const uint8_t* cbe_decode(cbe_decode_callbacks* callbacks, const uint8_t* const data_start, const uint8_t* const data_end);


#ifdef __cplusplus 
}
#endif
#endif // cbe_encoder_H
