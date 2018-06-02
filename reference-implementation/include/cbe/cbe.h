#ifndef cbe_encoder_H
#define cbe_encoder_H
#ifdef __cplusplus 
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>


#ifdef __cplusplus
// C++ doesn't seem to have a POD implementation of decimal types.
	#define CBE_HAS_DECIMAL_SUPPORT 0
#else
	#define CBE_HAS_DECIMAL_SUPPORT 1
#endif


// Encode

typedef struct {
	uint8_t* start;
	uint8_t* end;
	uint8_t* pos;
} cbe_encode_buffer;


const char* cbe_version();

void cbe_init_buffer(cbe_encode_buffer* const buffer, uint8_t* const memory_start, uint8_t* const memory_end);

bool cbe_add_boolean(cbe_encode_buffer* const buffer, const bool value);
bool cbe_add_int(cbe_encode_buffer* const buffer, const int value);
bool cbe_add_int_8(cbe_encode_buffer* const buffer, const int8_t value);
bool cbe_add_int_16(cbe_encode_buffer* const buffer, const int16_t value);
bool cbe_add_int_32(cbe_encode_buffer* const buffer, const int32_t value);
bool cbe_add_int_64(cbe_encode_buffer* const buffer, const int64_t value);
bool cbe_add_int_128(cbe_encode_buffer* const buffer, const __int128 value);
bool cbe_add_float_32(cbe_encode_buffer* const buffer, const float value);
bool cbe_add_float_64(cbe_encode_buffer* const buffer, const double value);
bool cbe_add_float_128(cbe_encode_buffer* const buffer, const long double value);
bool cbe_add_date(cbe_encode_buffer* const buffer, const unsigned year, const unsigned month, const unsigned day, const unsigned hour, const unsigned minute, const unsigned second);
bool cbe_add_timestamp_ms(cbe_encode_buffer* const buffer, const unsigned year, const unsigned month, const unsigned day, const unsigned hour, const unsigned minute, const unsigned second, const unsigned millisecond);
bool cbe_add_timestamp_us(cbe_encode_buffer* const buffer, const unsigned year, const unsigned month, const unsigned day, const unsigned hour, const unsigned minute, const unsigned second, const unsigned microsecond);
bool cbe_add_string(cbe_encode_buffer* const buffer, const char* const value);
bool cbe_add_bytes(cbe_encode_buffer* const buffer, const uint8_t* const value, const int length);
bool cbe_start_list(cbe_encode_buffer* const buffer);
bool cbe_start_map(cbe_encode_buffer* const buffer);
bool cbe_end_container(cbe_encode_buffer* const buffer);
bool cbe_add_array_boolean(cbe_encode_buffer* const buffer, const uint8_t* const packed_values, const int entity_count);
bool cbe_add_array_int_16(cbe_encode_buffer* const buffer, const int16_t* const values, const int entity_count);
bool cbe_add_array_int_32(cbe_encode_buffer* const buffer, const int32_t* const values, const int entity_count);
bool cbe_add_array_int_64(cbe_encode_buffer* const buffer, const int64_t* const values, const int entity_count);
bool cbe_add_array_int_128(cbe_encode_buffer* const buffer, const __int128* const values, const int entity_count);
bool cbe_add_array_float_32(cbe_encode_buffer* const buffer, const float* const values, const int entity_count);
bool cbe_add_array_float_64(cbe_encode_buffer* const buffer, const double* const values, const int entity_count);
bool cbe_add_array_float_128(cbe_encode_buffer* const buffer, const long double* const values, const int entity_count);

#if CBE_HAS_DECIMAL_SUPPORT
bool cbe_add_decimal_64(cbe_encode_buffer* const buffer, const _Decimal64 value);
bool cbe_add_decimal_128(cbe_encode_buffer* const buffer, const _Decimal128 value);
bool cbe_add_array_decimal_64(cbe_encode_buffer* const buffer, const _Decimal64* const values, const int entity_count);
bool cbe_add_array_decimal_128(cbe_encode_buffer* const buffer, const _Decimal128* const values, const int entity_count);
#endif


// Decode

typedef struct {
	void (*on_empty)(void);
	void (*on_bool)(bool value);
	void (*on_date)(int year, int month, int day, int hour, int minute, int second);
	void (*on_timestamp_ms)(int year, int month, int day, int hour, int minute, int second, int millisecond);
	void (*on_timestamp_us)(int year, int month, int day, int hour, int minute, int second, int microsecond);
	void (*on_end_container)(void);
	void (*on_list_start)(void);
	void (*on_map_start)(void);
	void (*on_string)(const int8_t* start, const int8_t* end);
	void (*on_bytes)(const uint8_t* start, const uint8_t* end);
	void (*on_int_8)(int8_t value);
	void (*on_int_16)(int16_t value);
	void (*on_int_32)(int32_t value);
	void (*on_int_64)(int64_t value);
	void (*on_int_128)(__int128 value);
	void (*on_float_32)(float value);
	void (*on_float_64)(double value);
	void (*on_float_128)(long double value);
#if CBE_HAS_DECIMAL_SUPPORT
	void (*on_decimal_64)(_Decimal64 value);
	void (*on_decimal_128)(_Decimal128 value); 
#endif
	void (*on_array_int_16)(const int16_t* start, const int16_t* end);
	void (*on_array_int_32)(const int32_t* start, const int32_t* end);
	void (*on_array_int_64)(const int64_t* start, const int64_t* end);
	void (*on_array_int_128)(const __int128* start, const __int128* end);
	void (*on_array_float_32)(const float* start, const float* end);
	void (*on_array_float_64)(const double* start, const double* end);
	void (*on_array_float_128)(const long double* start, const long double* end);
	// Array date, timestamps
	// Array bool
} cbe_decode_callbacks;


void cbe_decode(cbe_decode_callbacks* callbacks, const uint8_t* const data_start, const uint8_t* const data_end);


#ifdef __cplusplus 
}
#endif
#endif // cbe_encoder_H
