#include <stdlib.h>
#include "cbe/cbe.h"
#include "cbe_internal.h"



void cbe_decode(cbe_decode_callbacks* callbacks, const uint8_t* const data_start, const uint8_t* const data_end)
{
	for(const uint8_t* ptr = data_start; ptr < data_end;)
	{
		type_field type = *ptr++;
		switch(type)
		{
			case TYPE_EMPTY:
				callbacks->on_empty();
		        break;
			case TYPE_FALSE:
				callbacks->on_bool(false);
		        break;
			case TYPE_TRUE:
				callbacks->on_bool(true);
		        break;
			case TYPE_DATE:
			{
				callbacks->on_date(1,1,1,1,1,1);
		        break;
		    }
			case TYPE_TIMESTAMP_MILLI:
			{
				callbacks->on_timestamp_ms(1,1,1,1,1,1,1);
		        break;
		    }
			case TYPE_TIMESTAMP_MICRO:
			{
				callbacks->on_timestamp_us(1,1,1,1,1,1,1);
		        break;
		    }
			case TYPE_END_CONTAINER:
				callbacks->on_end_container();
		        break;
			case TYPE_LIST:
				callbacks->on_list_start();
		        break;
			case TYPE_MAP:
				callbacks->on_map_start();
		        break;
			case TYPE_ARRAY:
				// TODO
		        break;
			case TYPE_STRING:
				callbacks->on_string(NULL, NULL);
		        break;
			case TYPE_STRING_0:
			case TYPE_STRING_1:
			case TYPE_STRING_2:
			case TYPE_STRING_3:
			case TYPE_STRING_4:
			case TYPE_STRING_5:
			case TYPE_STRING_6:
			case TYPE_STRING_7:
			case TYPE_STRING_8:
			case TYPE_STRING_9:
			case TYPE_STRING_10:
			case TYPE_STRING_11:
			case TYPE_STRING_12:
			case TYPE_STRING_13:
			case TYPE_STRING_14:
			case TYPE_STRING_15:
			{
				int length = type - TYPE_STRING_0;
				callbacks->on_string((int8_t*)ptr, (int8_t*)(ptr + length));
				ptr += length;
		        break;
		    }
			case TYPE_BYTES_0:
			case TYPE_BYTES_1:
			case TYPE_BYTES_2:
			case TYPE_BYTES_3:
			case TYPE_BYTES_4:
			case TYPE_BYTES_5:
			case TYPE_BYTES_6:
			case TYPE_BYTES_7:
			case TYPE_BYTES_8:
			case TYPE_BYTES_9:
			case TYPE_BYTES_10:
			case TYPE_BYTES_11:
			case TYPE_BYTES_12:
			case TYPE_BYTES_13:
			case TYPE_BYTES_14:
			case TYPE_BYTES_15:
			{
				int length = type - TYPE_STRING_0;
				callbacks->on_bytes(ptr, ptr + length);
				ptr += length;
		        break;
		    }
			case TYPE_BYTES:
				callbacks->on_bytes(NULL, NULL);
		        break;
			case TYPE_INT_16:
				callbacks->on_int_16(0);
		        break;
			case TYPE_INT_32:
				callbacks->on_int_32(0);
		        break;
			case TYPE_INT_64:
				callbacks->on_int_64(0);
		        break;
			case TYPE_INT_128:
				callbacks->on_int_128(0);
		        break;
			case TYPE_FLOAT_32:
				callbacks->on_float_32(0);
		        break;
			case TYPE_FLOAT_64:
				callbacks->on_float_64(0);
		        break;
			case TYPE_FLOAT_128:
				callbacks->on_float_128(0);
		        break;
			case TYPE_DECIMAL_64:
				callbacks->on_decimal_64(0);
		        break;
			case TYPE_DECIMAL_128:
				callbacks->on_decimal_128(0);
		        break;
		    default:
		    	callbacks->on_int_8(type);
		    	break;
		}
	}
}
