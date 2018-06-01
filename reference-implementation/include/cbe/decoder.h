#ifndef cbe_decoder_H
#define cbe_decoder_H
#ifdef __cplusplus 
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	void (*on_empty)(void);
	void (*on_bool)(bool value);
	void (*on_date)(int year, int month, int day, int hour, int minute, int second);
	void (*on_timestamp_ms)(int year, int month, int day, int hour, int minute, int second, int ms);
	void (*on_timestamp_ns)(int year, int month, int day, int hour, int minute, int second, int ns);
	void (*on_end_container)(void);
	void (*on_list_start)(void);
	void (*on_map_start)(void);
	void (*on_string)(int8_t* start, int8_t* end);
	void (*on_bytes)(uint8_t* start, uint8_t* end);
	void (*on_int_8)(int8_t value);
	void (*on_int_16)(int16_t value);
	void (*on_int_32)(int32_t value);
	void (*on_int_64)(int64_t value);
	void (*on_int_128)(__int128 value);
	void (*on_float_32)(float value);
	void (*on_float_64)(double value);
	void (*on_float_128)(long double value);
	void (*on_decimal_64)(_Decimal64 value);
	void (*on_decimal_128)(_Decimal128 value); 
	void (*on_array_int_16)(int16_t* start, int16_t* end);
	void (*on_array_int_32)(int32_t* start, int32_t* end);
	void (*on_array_int_64)(int64_t* start, int64_t* end);
	void (*on_array_int_128)(__int128* start, __int128* end);
	void (*on_array_float_32)(float* start, float* end);
	void (*on_array_float_64)(double* start, double* end);
	void (*on_array_float_128)(long double* start, long double* end);
	// Array date, timestamps
	// Array bool
} cbe_decode_callbacks;


void cbe_decode(cbe_decode_callbacks* callbacks, const uint8_t* const data_start, const uint8_t* const data_end);


#ifdef __cplusplus 
}
#endif
#endif // cbe_decoder_H
