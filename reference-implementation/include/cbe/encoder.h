#ifndef cbe_encoder_H
#define cbe_encoder_H
#ifdef __cplusplus 
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint8_t* start;
	uint8_t* end;
	uint8_t* pos;
} cbe_buffer;


const char* cbe_version();

void cbe_init_buffer(cbe_buffer* buffer, uint8_t* memory_start, uint8_t* memory_end);

bool cbe_add_boolean(cbe_buffer* buffer, bool value);
bool cbe_add_int(cbe_buffer* buffer, int value);
bool cbe_add_int8(cbe_buffer* buffer, int8_t value);
bool cbe_add_int16(cbe_buffer* buffer, int16_t value);
bool cbe_add_int32(cbe_buffer* buffer, int32_t value);
bool cbe_add_int64(cbe_buffer* buffer, int64_t value);
bool cbe_add_int128(cbe_buffer* buffer, __int128 value);
bool cbe_add_float32(cbe_buffer* buffer, float value);
bool cbe_add_float64(cbe_buffer* buffer, double value);
bool cbe_add_float128(cbe_buffer* buffer, long double value);
bool cbe_add_date(cbe_buffer* buffer, unsigned year, unsigned month, unsigned day, unsigned hour, unsigned minute, unsigned second);
bool cbe_add_timestamp(cbe_buffer* buffer, unsigned year, unsigned month, unsigned day, unsigned hour, unsigned minute, unsigned second, unsigned msec);
bool cbe_add_timestamp_ns(cbe_buffer* buffer, unsigned year, unsigned month, unsigned day, unsigned hour, unsigned minute, unsigned second, unsigned nsec);
bool cbe_add_string(cbe_buffer* buffer, const char* const value);
bool cbe_add_bytes(cbe_buffer* buffer, const uint8_t* const value, int length);
bool cbe_start_list(cbe_buffer* buffer);
bool cbe_start_map(cbe_buffer* buffer);
bool cbe_end_container(cbe_buffer* buffer);
bool cbe_add_boolean_array(cbe_buffer* buffer, uint8_t* packed_values, int entity_count);
bool cbe_add_int16_array(cbe_buffer* const buffer, const int16_t* const values, const int entity_count);
bool cbe_add_int32_array(cbe_buffer* const buffer, const int32_t* const values, const int entity_count);
bool cbe_add_int64_array(cbe_buffer* const buffer, const int64_t* const values, const int entity_count);
bool cbe_add_int128_array(cbe_buffer* const buffer, const __int128* const values, const int entity_count);
bool cbe_add_float32_array(cbe_buffer* const buffer, const float* const values, const int entity_count);
bool cbe_add_float64_array(cbe_buffer* const buffer, const double* const values, const int entity_count);
bool cbe_add_float128_array(cbe_buffer* const buffer, const long double* const values, const int entity_count);

#if CBE_HAS_DECIMAL_SUPPORT
bool cbe_add_decimal64(cbe_buffer* buffer, _Decimal64 value);
bool cbe_add_decimal128(cbe_buffer* buffer, _Decimal128 value);
bool cbe_add_decimal64_array(cbe_buffer* const buffer, const _Decimal64* const values, const int entity_count);
bool cbe_add_decimal128_array(cbe_buffer* const buffer, const _Decimal128* const values, const int entity_count);
#endif


#ifdef __cplusplus 
}
#endif
#endif // cbe_encoder_H
