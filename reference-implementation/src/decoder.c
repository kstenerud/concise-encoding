#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include "cbe/cbe.h"
#include "cbe_internal.h"

typedef struct
{
	const uint8_t* const start;
	const uint8_t* const end;
	const uint8_t* pos;
} ro_buffer;

ro_buffer new_buffer(const uint8_t* const memory_start, const uint8_t* const memory_end)
{
	ro_buffer buffer =
	{
		.start = memory_start,
		.pos = memory_start,
		.end = memory_end
	};
	return buffer;
}


static inline uint8_t peek_uint_8(ro_buffer* const buffer)
{
	return *buffer->pos;
}

static inline uint8_t read_uint_8(ro_buffer* const buffer)
{
	return *buffer->pos++;
}

static inline int8_t read_int_8(ro_buffer* const buffer)
{
	return (int8_t)*buffer->pos++;
}

#define DEFINE_READ_UINT_IRREGULAR_FUNCTION(WIDTH) \
static inline uint64_t read_uint_ ## WIDTH(ro_buffer* const buffer) \
{ \
	const int byte_count = WIDTH/8; \
	uint64_t value = 0; \
	for(const uint8_t* pos = buffer->pos + byte_count - 1; pos >= buffer->pos; pos--) \
	{ \
		value = (value << 8) | *pos; \
	} \
	buffer->pos += byte_count; \
	return value; \
}
DEFINE_READ_UINT_IRREGULAR_FUNCTION(40)
DEFINE_READ_UINT_IRREGULAR_FUNCTION(48)

#define DEFINE_READ_FUNCTION(TYPE, TYPE_SUFFIX) \
static inline TYPE read_ ## TYPE_SUFFIX(ro_buffer* const buffer) \
{ \
	const safe_ ## TYPE_SUFFIX* const safe = (safe_ ## TYPE_SUFFIX*)buffer->pos; \
	buffer->pos += sizeof(safe); \
	return safe->contents; \
}
DEFINE_READ_FUNCTION(uint16_t,    uint_16)
DEFINE_READ_FUNCTION(uint32_t,    uint_32)
DEFINE_READ_FUNCTION(uint64_t,    uint_64)
DEFINE_READ_FUNCTION(int16_t,     int_16)
DEFINE_READ_FUNCTION(int32_t,     int_32)
DEFINE_READ_FUNCTION(int64_t,     int_64)
DEFINE_READ_FUNCTION(__int128,    int_128)
DEFINE_READ_FUNCTION(float,       float_32)
DEFINE_READ_FUNCTION(double,      float_64)
DEFINE_READ_FUNCTION(long double, float_128)
DEFINE_READ_FUNCTION(_Decimal64,  decimal_64)
DEFINE_READ_FUNCTION(_Decimal128, decimal_128)

static inline uint64_t get_array_length(ro_buffer* const buffer)
{
	switch(peek_uint_8(buffer) & 3)
	{
		case LENGTH_FIELD_WIDTH_6_BIT:
			return read_uint_8(buffer) >> 2;
		case LENGTH_FIELD_WIDTH_14_BIT:
			return read_uint_16(buffer) >> 2;
		case LENGTH_FIELD_WIDTH_30_BIT:
			return read_uint_32(buffer) >> 2;
		default:
			return read_uint_64(buffer) >> 2;
	}
}

// static void report_error(cbe_decode_callbacks* callbacks, char* fmt, ...)
// {
// 	va_list args;
// 	va_start(args, fmt);
// 	char buff[200];
// 	vsprintf(buff, fmt, args);
// 	callbacks->on_error(buff);
// 	va_end(args);
// }

const uint8_t* cbe_decode(cbe_decode_callbacks* callbacks, const uint8_t* const data_start, const uint8_t* const data_end)
{
	ro_buffer real_buffer = new_buffer(data_start, data_end);
	ro_buffer* buffer = &real_buffer;
	while(buffer->pos < buffer->end)
	{
		type_field type = read_uint_8(buffer);
		switch(type)
		{
			case TYPE_PADDING:
				// Ignore
				break;
			case TYPE_EMPTY:
				if(!callbacks->on_empty()) return NULL;
				break;
			case TYPE_FALSE:
				if(!callbacks->on_bool(false)) return NULL;
		        break;
			case TYPE_TRUE:
				if(!callbacks->on_bool(true)) return NULL;
		        break;
			case TYPE_END_CONTAINER:
				if(!callbacks->on_end_container()) return NULL;
		        break;
			case TYPE_LIST:
				if(!callbacks->on_list_start()) return NULL;
		        break;
			case TYPE_MAP:
				if(!callbacks->on_map_start()) return NULL;
		        break;

		    #define HANDLE_CASE_DATE(VALUE) \
			{ \
				uint64_t encoded = VALUE; \
				cbe_date date = \
				{ \
					.year = encoded / DATE_MULTIPLIER_YEAR, \
					.month = (encoded / DATE_MULTIPLIER_MONTH) % DATE_MODULO_MONTH + 1, \
					.day = (encoded / DATE_MULTIPLIER_DAY) % DATE_MODULO_DAY + 1, \
					.hour = (encoded / DATE_MULTIPLIER_HOUR) % DATE_MODULO_HOUR, \
					.minute = (encoded / DATE_MULTIPLIER_MINUTE) % DATE_MODULO_MINUTE, \
					.second = (encoded / DATE_MULTIPLIER_SECOND) % DATE_MODULO_SECOND, \
					.microsecond = (encoded / DATE_MULTIPLIER_MICROSECOND) % (DATE_MODULO_MILLISECOND * DATE_MODULO_MICROSECOND), \
				}; \
				if(!callbacks->on_date(&date)) return NULL; \
		    }
			case TYPE_DATE_40:
				HANDLE_CASE_DATE(read_uint_40(buffer) * DATE_MULTIPLIER_MILLISECOND * DATE_MULTIPLIER_MICROSECOND)
		        break;
			case TYPE_DATE_48:
				HANDLE_CASE_DATE(read_uint_48(buffer) * DATE_MULTIPLIER_MICROSECOND)
		        break;
			case TYPE_DATE_64:
				HANDLE_CASE_DATE(read_uint_64(buffer))
		        break;

			case TYPE_STRING_0: case TYPE_STRING_1: case TYPE_STRING_2: case TYPE_STRING_3:
			case TYPE_STRING_4: case TYPE_STRING_5: case TYPE_STRING_6: case TYPE_STRING_7:
			case TYPE_STRING_8: case TYPE_STRING_9: case TYPE_STRING_10: case TYPE_STRING_11:
			case TYPE_STRING_12: case TYPE_STRING_13: case TYPE_STRING_14: case TYPE_STRING_MAX:
			{
				uint64_t length = type - TYPE_STRING_0;
				if(!callbacks->on_string((char*)buffer->pos, (char*)buffer->pos + length)) return NULL;
				buffer->pos += length;
		        break;
		    }

		    #define HANDLE_CASE_SCALAR(TYPE_UPPER, TYPE_LOWER) \
			{ \
		    	cbe_number number = \
		    	{ \
		    		.data = {.TYPE_LOWER = read_ ## TYPE_LOWER(buffer)}, \
		    		.type = CBE_NUMERIC_ ## TYPE_UPPER \
		    	}; \
		    	if(!callbacks->on_number(number)) return NULL; \
		    }
			case TYPE_INT_16:
				HANDLE_CASE_SCALAR(INT_16, int_16)
		        break;
			case TYPE_INT_32:
				HANDLE_CASE_SCALAR(INT_32, int_32)
				break;
			case TYPE_INT_64:
				HANDLE_CASE_SCALAR(INT_64, int_64)
				break;
			case TYPE_INT_128:
				HANDLE_CASE_SCALAR(INT_128, int_128)
				break;
			case TYPE_FLOAT_32:
				HANDLE_CASE_SCALAR(FLOAT_32, float_32)
				break;
			case TYPE_FLOAT_64:
				HANDLE_CASE_SCALAR(FLOAT_64, float_64)
				break;
			case TYPE_FLOAT_128:
				HANDLE_CASE_SCALAR(FLOAT_128, float_128)
				break;
			case TYPE_DECIMAL_64:
				HANDLE_CASE_SCALAR(DECIMAL_64, decimal_64)
				break;
			case TYPE_DECIMAL_128:
				HANDLE_CASE_SCALAR(DECIMAL_128, decimal_128)
				break;

			#define HANDLE_CASE_ARRAY(TYPE, HANDLER) \
			{ \
				uint64_t length = get_array_length(buffer); \
				if(!callbacks->HANDLER((TYPE*)buffer->pos, (TYPE*)buffer->pos + length * sizeof(TYPE))) return NULL; \
				buffer->pos += length * sizeof(TYPE); \
			}
			case TYPE_ARRAY_STRING:
				HANDLE_CASE_ARRAY(char, on_string);
				break;
			case TYPE_ARRAY_INT_8:
				HANDLE_CASE_ARRAY(int8_t, on_array_int_8);
				break;
			case TYPE_ARRAY_INT_16:
				HANDLE_CASE_ARRAY(int16_t, on_array_int_16);
				break;
			case TYPE_ARRAY_INT_32:
				HANDLE_CASE_ARRAY(int32_t, on_array_int_32);
				break;
			case TYPE_ARRAY_INT_64:
				HANDLE_CASE_ARRAY(int64_t, on_array_int_64);
				break;
			case TYPE_ARRAY_INT_128:
				HANDLE_CASE_ARRAY(__int128, on_array_int_128);
				break;
			case TYPE_ARRAY_FLOAT_32:
				HANDLE_CASE_ARRAY(float, on_array_float_32);
				break;
			case TYPE_ARRAY_FLOAT_64:
				HANDLE_CASE_ARRAY(double, on_array_float_64);
				break;
			case TYPE_ARRAY_FLOAT_128:
				HANDLE_CASE_ARRAY(long double, on_array_float_128);
				break;
			case TYPE_ARRAY_DECIMAL_64:
				HANDLE_CASE_ARRAY(_Decimal64, on_array_decimal_64);
				break;
			case TYPE_ARRAY_DECIMAL_128:
				HANDLE_CASE_ARRAY(_Decimal128, on_array_decimal_128);
				break;
			case TYPE_ARRAY_DATE:
				HANDLE_CASE_ARRAY(uint64_t, on_array_date);
				break;
			case TYPE_ARRAY_BOOLEAN:
			{
				uint64_t length = get_array_length(buffer);
				if(!callbacks->on_bitfield(buffer->pos, length)) return NULL;
				uint64_t bytes = length / 8;
				if(bytes & 7)
				{
					bytes++;
				}
				buffer->pos += bytes;
				break;
			}
		    default:
		    {
		    	cbe_number number =
		    	{
		    		.data = {.int_8 = type},
		    		.type = CBE_NUMERIC_INT_8
		    	};
		    	if(!callbacks->on_number(number)) return NULL;
		    	break;
		    }
		}
	}
	return buffer->pos;
}
