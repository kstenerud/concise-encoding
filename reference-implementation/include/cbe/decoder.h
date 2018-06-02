#ifndef cbe_decoder_H
#define cbe_decoder_H
#ifdef __cplusplus 
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "cbe_config.h"

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
#endif // cbe_decoder_H
