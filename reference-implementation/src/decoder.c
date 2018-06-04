#include <stdlib.h>
#include "cbe/cbe.h"
#include "cbe_internal.h"

static inline uint8_t read_uint_8(cbe_buffer* const buffer)
{
	return *buffer->pos++;
}

static inline int8_t read_int_8(cbe_buffer* const buffer)
{
	return (int8_t)*buffer->pos++;
}

#define DEFINE_READ_UINT_IRREGULAR_FUNCTION(WIDTH) \
static inline uint64_t read_uint_ ## WIDTH(cbe_buffer* const buffer) \
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
static inline TYPE read_ ## TYPE_SUFFIX(cbe_buffer* const buffer) \
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

static inline uint64_t read_bytes_length(cbe_buffer* const buffer)
{
	uint8_t length = read_uint_8(buffer);
	switch(length)
	{
		case BYTES_LENGTH_16_BIT:
			return read_uint_16(buffer);
		case BYTES_LENGTH_32_BIT:
			return read_uint_32(buffer);
		case BYTES_LENGTH_64_BIT:
			return read_uint_64(buffer);
		default:
			return length;
	}
}

static inline array_content_type_field get_array_content_type(cbe_buffer* const buffer)
{
	return *buffer->pos & ARRAY_CONTENT_TYPE_MASK;
}

static inline uint64_t get_array_length(cbe_buffer* const buffer)
{
	uint8_t length = read_uint_8(buffer) & ~ARRAY_CONTENT_TYPE_MASK;
	switch(length)
	{
		case ARRAY_LENGTH_8_BIT:
			return read_uint_8(buffer);
		case ARRAY_LENGTH_16_BIT:
			return read_uint_16(buffer);
		case ARRAY_LENGTH_32_BIT:
			return read_uint_32(buffer);
		case ARRAY_LENGTH_64_BIT:
			return read_uint_64(buffer);
		default:
			return length;
	}
}

static void decode_array(cbe_buffer* buffer, cbe_decode_callbacks* callbacks)
{
	array_content_type_field type = get_array_content_type(buffer);
	uint64_t length = get_array_length(buffer);
	switch(type)
	{
		#define DEFINE_DECODE_ARRAY_CASE_HANDLER(TYPE, SUFFIX) \
			callbacks->on_array_ ## SUFFIX((TYPE*)buffer->pos, (TYPE*)buffer->pos + length * sizeof(TYPE)); \
			buffer->pos += length * sizeof(TYPE)

		case ARRAY_CONTENT_TYPE_BOOLEAN:
			break;
		case ARRAY_CONTENT_TYPE_INT_16:
			DEFINE_DECODE_ARRAY_CASE_HANDLER(int16_t, int_16);
			break;
		case ARRAY_CONTENT_TYPE_INT_32:
			DEFINE_DECODE_ARRAY_CASE_HANDLER(int32_t, int_32);
			break;
		case ARRAY_CONTENT_TYPE_INT_64:
			DEFINE_DECODE_ARRAY_CASE_HANDLER(int64_t, int_64);
			break;
		case ARRAY_CONTENT_TYPE_INT_128:
			DEFINE_DECODE_ARRAY_CASE_HANDLER(__int128, int_128);
			break;
		case ARRAY_CONTENT_TYPE_FLOAT_32:
			DEFINE_DECODE_ARRAY_CASE_HANDLER(float, float_32);
			break;
		case ARRAY_CONTENT_TYPE_FLOAT_64:
			DEFINE_DECODE_ARRAY_CASE_HANDLER(double, float_64);
			break;
		case ARRAY_CONTENT_TYPE_FLOAT_128:
			DEFINE_DECODE_ARRAY_CASE_HANDLER(long double, float_128);
			break;
		case ARRAY_CONTENT_TYPE_DECIMAL_64:
			DEFINE_DECODE_ARRAY_CASE_HANDLER(_Decimal64, decimal_64);
			break;
		case ARRAY_CONTENT_TYPE_DECIMAL_128:
			DEFINE_DECODE_ARRAY_CASE_HANDLER(_Decimal128, decimal_128);
			break;
		case ARRAY_CONTENT_TYPE_DATE:
			// TODO
			break;
		case ARRAY_CONTENT_TYPE_TIMESTAMP_MS:
			// TODO
			break;
		case ARRAY_CONTENT_TYPE_TIMESTAMP_NS:
			// TODO
			break;
		default:
			// TODO: Error
			break;
	}
}

void cbe_decode(cbe_decode_callbacks* callbacks, uint8_t* const data_start, uint8_t* const data_end)
{
	cbe_buffer real_buffer;
	cbe_buffer* buffer = &real_buffer;
	cbe_init_buffer(buffer, data_start, data_end);
	while(buffer->pos < buffer->end)
	{
		type_field type = read_uint_8(buffer);
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
				uint64_t encoded = read_uint_40(buffer);
				cbe_date date =
				{
					.year = encoded / DATE_MULTIPLIER_YEAR,
					.month = (encoded / DATE_MULTIPLIER_MONTH) % DATE_MODULO_MONTH + 1,
					.day = (encoded / DATE_MULTIPLIER_DAY) % DATE_MODULO_DAY + 1,
					.hour = (encoded / DATE_MULTIPLIER_HOUR) % DATE_MODULO_HOUR,
					.minute = (encoded / DATE_MULTIPLIER_MINUTE) % DATE_MODULO_MINUTE,
					.second = (encoded / DATE_MULTIPLIER_SECOND) % DATE_MODULO_SECOND,
				};
				callbacks->on_date(&date);
		        break;
		    }
			case TYPE_TIMESTAMP_MILLI:
			{
				uint64_t encoded = read_uint_48(buffer);
				cbe_date date =
				{
					.year = encoded / TS_MS_MULTIPLIER_YEAR,
					.month = (encoded / TS_MS_MULTIPLIER_MONTH) % DATE_MODULO_MONTH + 1,
					.day = (encoded / TS_MS_MULTIPLIER_DAY) % DATE_MODULO_DAY + 1,
					.hour = (encoded / TS_MS_MULTIPLIER_HOUR) % DATE_MODULO_HOUR,
					.minute = (encoded / TS_MS_MULTIPLIER_MINUTE) % DATE_MODULO_MINUTE,
					.second = (encoded / TS_MS_MULTIPLIER_SECOND) % DATE_MODULO_SECOND,
					.microsecond = ((encoded / TS_MS_MULTIPLIER_MILLISECOND) % DATE_MODULO_MILLISECOND) * 1000,
				};
				callbacks->on_date(&date);
		        break;
		    }
			case TYPE_TIMESTAMP_MICRO:
			{
				uint64_t encoded = read_uint_64(buffer);
				cbe_date date =
				{
					.year = encoded / TS_US_MULTIPLIER_YEAR,
					.month = (encoded / TS_US_MULTIPLIER_MONTH) % DATE_MODULO_MONTH + 1,
					.day = (encoded / TS_US_MULTIPLIER_DAY) % DATE_MODULO_DAY + 1,
					.hour = (encoded / TS_US_MULTIPLIER_HOUR) % DATE_MODULO_HOUR,
					.minute = (encoded / TS_US_MULTIPLIER_MINUTE) % DATE_MODULO_MINUTE,
					.second = (encoded / TS_US_MULTIPLIER_SECOND) % DATE_MODULO_SECOND,
					.microsecond = (encoded / TS_US_MULTIPLIER_MICROSECOND) % DATE_MODULO_MICROSECOND,
				};
				callbacks->on_date(&date);
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
				decode_array(buffer, callbacks);
		        break;
			case TYPE_STRING:
			{
				uint64_t length = read_bytes_length(buffer);
				// TODO: check for end of buffer
				callbacks->on_string((int8_t*)buffer->pos, (int8_t*)buffer->pos+length);
				buffer->pos += length;
		        break;
		    }
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
				callbacks->on_string((int8_t*)buffer->pos, (int8_t*)(buffer->pos + length));
				buffer->pos += length;
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
				callbacks->on_bytes(buffer->pos, buffer->pos + length);
				buffer->pos += length;
		        break;
		    }
			case TYPE_BYTES:
			{
				uint64_t length = read_bytes_length(buffer);
				// TODO: check for end of buffer
				callbacks->on_bytes(buffer->pos, buffer->pos+length);
		        break;
		    }
			case TYPE_INT_16:
			{
		    	cbe_number number =
		    	{
		    		.data = {.int_16 = read_int_16(buffer)},
		    		.type = CBE_NUMERIC_INT_16
		    	};
		    	callbacks->on_number(number);
		        break;
		    }
			case TYPE_INT_32:
			{
		    	cbe_number number =
		    	{
		    		.data = {.int_32 = read_int_32(buffer)},
		    		.type = CBE_NUMERIC_INT_32
		    	};
		    	callbacks->on_number(number);
		        break;
		    }
			case TYPE_INT_64:
			{
		    	cbe_number number =
		    	{
		    		.data = {.int_64 = read_int_64(buffer)},
		    		.type = CBE_NUMERIC_INT_64
		    	};
		    	callbacks->on_number(number);
		        break;
		    }
			case TYPE_INT_128:
			{
		    	cbe_number number =
		    	{
		    		.data = {.int_128 = read_int_128(buffer)},
		    		.type = CBE_NUMERIC_INT_128
		    	};
		    	callbacks->on_number(number);
		        break;
		    }
			case TYPE_FLOAT_32:
			{
		    	cbe_number number =
		    	{
		    		.data = {.float_32 = read_float_32(buffer)},
		    		.type = CBE_NUMERIC_FLOAT_32
		    	};
		    	callbacks->on_number(number);
		        break;
		    }
			case TYPE_FLOAT_64:
			{
		    	cbe_number number =
		    	{
		    		.data = {.float_64 = read_float_64(buffer)},
		    		.type = CBE_NUMERIC_FLOAT_64
		    	};
		    	callbacks->on_number(number);
		        break;
		    }
			case TYPE_FLOAT_128:
			{
		    	cbe_number number =
		    	{
		    		.data = {.float_128 = read_float_128(buffer)},
		    		.type = CBE_NUMERIC_FLOAT_128
		    	};
		    	callbacks->on_number(number);
		        break;
		    }
			case TYPE_DECIMAL_64:
			{
		    	cbe_number number =
		    	{
		    		.data = {.decimal_64 = read_decimal_64(buffer)},
		    		.type = CBE_NUMERIC_DECIMAL_64
		    	};
		    	callbacks->on_number(number);
		        break;
		    }
			case TYPE_DECIMAL_128:
			{
		    	cbe_number number =
		    	{
		    		.data = {.decimal_128 = read_decimal_128(buffer)},
		    		.type = CBE_NUMERIC_DECIMAL_128
		    	};
		    	callbacks->on_number(number);
		        break;
		    }
		    default:
		    {
		    	cbe_number number =
		    	{
		    		.data = {.int_8 = type},
		    		.type = CBE_NUMERIC_INT_8
		    	};
		    	callbacks->on_number(number);
		    	break;
		    }
		}
	}
}
