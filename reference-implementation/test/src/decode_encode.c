#include <stdlib.h>
#include <cbe/cbe.h>
#include "decode_encode.h"

static cbe_buffer* g_buffer;

static bool on_error(const char* message)
{
	// TODO: Store this somewhere for the tests to pick up?
	return false;
}

static bool on_empty(void)
{
	return cbe_add_empty(g_buffer);
}

static bool on_bool(bool value)
{
	return cbe_add_boolean(g_buffer, value);
}

static bool on_number(cbe_number value)
{
	switch(value.type)
	{
		case CBE_NUMERIC_INT_8:
			return cbe_add_int_8(g_buffer, value.data.int_8);
		case CBE_NUMERIC_INT_16:
			return cbe_add_int_16(g_buffer, value.data.int_16);
		case CBE_NUMERIC_INT_32:
			return cbe_add_int_32(g_buffer, value.data.int_32);
		case CBE_NUMERIC_INT_64:
			return cbe_add_int_64(g_buffer, value.data.int_64);
		case CBE_NUMERIC_INT_128:
			return cbe_add_int_128(g_buffer, value.data.int_128);
		case CBE_NUMERIC_FLOAT_32:
			return cbe_add_float_32(g_buffer, value.data.float_32);
		case CBE_NUMERIC_FLOAT_64:
			return cbe_add_float_64(g_buffer, value.data.float_64);
		case CBE_NUMERIC_FLOAT_128:
			return cbe_add_float_128(g_buffer, value.data.float_128);
#if CBE_HAS_DECIMAL_SUPPORT
		case CBE_NUMERIC_DECIMAL_64:
			return cbe_add_decimal_64(g_buffer, value.data.decimal_64);
		case CBE_NUMERIC_DECIMAL_128:
			// TODO
			break;
#endif
	}
	return true;
}

bool on_date(cbe_date* value)
{
	return cbe_add_date(g_buffer, value);
}

bool on_end_container(void)
{
	return cbe_end_container(g_buffer);
}

bool on_list_start(void)
{
	return cbe_start_list(g_buffer);
}

bool on_map_start(void)
{
	return cbe_start_map(g_buffer);
}

bool on_bitfield(const uint8_t* start, const uint64_t length)
{
	return cbe_add_bitfield(g_buffer, start, length);
}

bool on_string(const char* start, const char* end)
{
	return cbe_add_string(g_buffer, start, end - start);
}

bool on_array_int_8(const int8_t* start, const int8_t* end)
{
	return cbe_add_array_int_8(g_buffer, start, end - start);
}

bool on_array_int_16(const int16_t* start, const int16_t* end)
{
	return cbe_add_array_int_16(g_buffer, start, end - start);
}

bool on_array_int_32(const int32_t* start, const int32_t* end)
{
	return cbe_add_array_int_32(g_buffer, start, end - start);
}

bool on_array_int_64(const int64_t* start, const int64_t* end)
{
	return cbe_add_array_int_64(g_buffer, start, end - start);
}

bool on_array_int_128(const __int128* start, const __int128* end)
{
	return cbe_add_array_int_128(g_buffer, start, end - start);
}

bool on_array_float_32(const float* start, const float* end)
{
	return cbe_add_array_float_32(g_buffer, start, end - start);
}

bool on_array_float_64(const double* start, const double* end)
{
	return cbe_add_array_float_64(g_buffer, start, end - start);
}

bool on_array_float_128(const long double* start, const long double* end)
{
	return cbe_add_array_float_128(g_buffer, start, end - start);
}

#if CBE_HAS_DECIMAL_SUPPORT
bool on_array_decimal_64(const _Decimal64* start, const _Decimal64* end)
{
	return cbe_add_array_decimal_64(g_buffer, start, end - start);
}

bool on_array_decimal_128(const _Decimal128* start, const _Decimal128* end)
{
	return cbe_add_array_decimal_128(g_buffer, start, end - start);
}
#endif

bool on_array_date(const uint64_t* start, const uint64_t* end)
{
	// TODO
	// return cbe_add_array_date(g_buffer, start, end - start);
}

bool decode_encode(const uint8_t* src, int src_length, cbe_buffer* dst_buffer)
{
	cbe_decode_callbacks callbacks;
	callbacks.on_error = on_error;
	callbacks.on_empty = on_empty;
	callbacks.on_bool = on_bool;
	callbacks.on_number = on_number;
	callbacks.on_date = on_date;
	callbacks.on_end_container = on_end_container;
	callbacks.on_list_start = on_list_start;
	callbacks.on_map_start = on_map_start;
	callbacks.on_bitfield = on_bitfield;
	callbacks.on_string = on_string;
	callbacks.on_array_int_8 = on_array_int_8;
	callbacks.on_array_int_16 = on_array_int_16;
	callbacks.on_array_int_32 = on_array_int_32;
	callbacks.on_array_int_64 = on_array_int_64;
	callbacks.on_array_int_128 = on_array_int_128;
	callbacks.on_array_float_32 = on_array_float_32;
	callbacks.on_array_float_64 = on_array_float_64;
	callbacks.on_array_float_128 = on_array_float_128;
#if CBE_HAS_DECIMAL_SUPPORT
	callbacks.on_array_decimal_64 = on_array_decimal_64;
	callbacks.on_array_decimal_128 = on_array_decimal_128;
#endif
	callbacks.on_array_date = on_array_date;

	g_buffer = dst_buffer;
	return cbe_decode(&callbacks, src, src + src_length) != NULL;
}
