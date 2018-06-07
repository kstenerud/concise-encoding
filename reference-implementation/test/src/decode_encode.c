#include <stdlib.h>
#include <cbe/cbe.h>
#include "decode_encode.h"

static cbe_buffer* g_buffer;

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



bool decode_encode(const uint8_t* src, int src_length, cbe_buffer* dst_buffer)
{
	cbe_decode_callbacks callbacks;
	callbacks.on_number = on_number;

	g_buffer = dst_buffer;
	return cbe_decode(&callbacks, src, src + src_length) != NULL;
}
