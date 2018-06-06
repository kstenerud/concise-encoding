#include <cbe/cbe.h>
#include "roundtrip_reencode.h"

static cbe_buffer g_buffer;

static bool on_number(cbe_number value)
{
	switch(value.type)
	{
		case CBE_NUMERIC_INT_8:
			cbe_add_int_8(&g_buffer, value.data.int_8);
			break;
		case CBE_NUMERIC_INT_16:
			cbe_add_int_16(&g_buffer, value.data.int_16);
			break;
		case CBE_NUMERIC_INT_32:
			cbe_add_int_32(&g_buffer, value.data.int_32);
			break;
		case CBE_NUMERIC_INT_64:
			cbe_add_int_64(&g_buffer, value.data.int_64);
			break;
		case CBE_NUMERIC_INT_128:
			cbe_add_int_128(&g_buffer, value.data.int_128);
			break;
		case CBE_NUMERIC_FLOAT_32:
			cbe_add_float_32(&g_buffer, value.data.float_32);
			break;
		case CBE_NUMERIC_FLOAT_64:
			cbe_add_float_64(&g_buffer, value.data.float_64);
			break;
		case CBE_NUMERIC_FLOAT_128:
			cbe_add_float_128(&g_buffer, value.data.float_128);
			break;
#if CBE_HAS_DECIMAL_SUPPORT
		case CBE_NUMERIC_DECIMAL_64:
			break;
		case CBE_NUMERIC_DECIMAL_128:
			break;
#endif
	}
	return true;
}



void roundtrip_reencode(const uint8_t* src, int src_length, cbe_buffer* dst_buffer)
{
	cbe_decode_callbacks callbacks;
	callbacks.on_number = on_number;

	cbe_init_buffer(&g_buffer, dst_buffer->start, dst_buffer->end);
	cbe_decode(&callbacks, src, src + src_length);
	*dst_buffer = g_buffer;
}
