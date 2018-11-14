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



// ------------
// Encoding API
// ------------

/**
 * Holds metadata about a buffer.
 */
typedef struct
{
	const uint8_t* const start;
	const uint8_t* const end;
	uint8_t* pos;
} cbe_buffer;

typedef struct
{
	int year;
	unsigned int day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	unsigned int microsecond;
} cbe_time;


/**
 * Get the current library version.
 *
 * @return The library version.
 */
const char* cbe_version();

/**
 * Create a new buffer metadata object.
 *
 * @param memory_start The start of the buffer's memory.
 * @param memory_end The end of the buffer's memory.
 * @return The new buffer.
 */
cbe_buffer cbe_new_buffer(uint8_t* const memory_start, uint8_t* const memory_end);

/**
 * Add an empty object to the buffer.
 *
 * @param buffer The buffer to add to.
 * @return true if the operation was successful.
 */
bool cbe_add_empty(cbe_buffer* const buffer);

/**
 * Add a boolean value to the buffer.
 *
 * @param buffer The buffer to add to.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_boolean(cbe_buffer* const buffer, const bool value);

/**
 * Add an integer value to the buffer.
 *
 * @param buffer The buffer to add to.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_int(cbe_buffer* const buffer, const int value);

/**
 * Add an integer value to the buffer.
 *
 * @param buffer The buffer to add to.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_int_8(cbe_buffer* const buffer, const int8_t value);

/**
 * Add a 16 bit integer value to the buffer.
 * Note that this will add a narrower type if it will fit.
 *
 * @param buffer The buffer to add to.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_int_16(cbe_buffer* const buffer, const int16_t value);

/**
 * Add a 32 bit integer value to the buffer.
 * Note that this will add a narrower type if it will fit.
 *
 * @param buffer The buffer to add to.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_int_32(cbe_buffer* const buffer, const int32_t value);

/**
 * Add a 64 bit integer value to the buffer.
 * Note that this will add a narrower type if it will fit.
 *
 * @param buffer The buffer to add to.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_int_64(cbe_buffer* const buffer, const int64_t value);

/**
 * Add a 128 bit integer value to the buffer.
 * Note that this will add a narrower type if it will fit.
 *
 * @param buffer The buffer to add to.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_int_128(cbe_buffer* const buffer, const __int128 value);

/**
 * Add a 32 bit floating point value to the buffer.
 * Note that this will add a narrower type if it will fit.
 *
 * @param buffer The buffer to add to.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_float_32(cbe_buffer* const buffer, const float value);

/**
 * Add a 64 bit floating point value to the buffer.
 * Note that this will add a narrower type if it will fit.
 *
 * @param buffer The buffer to add to.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_float_64(cbe_buffer* const buffer, const double value);

/**
 * Add a 128 bit floating point value to the buffer.
 * Note that this will add a narrower type if it will fit.
 *
 * @param buffer The buffer to add to.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_float_128(cbe_buffer* const buffer, const __float128 value);

/**
 * Add a time value to the buffer.
 * The storage width depends on what sub-second precision is present in the time.
 *
 * @param buffer The buffer to add to.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_time(cbe_buffer* const buffer, const cbe_time* const value);

/**
 * Add a UTF-8 encoded string value to the buffer.
 * Do not include a byte order marker (BOM)
 *
 * @param buffer The buffer to add to.
 * @param str The string to add.
 * @param byte_count The byte length of the string.
 * @return true if the operation was successful.
 */
bool cbe_add_string(cbe_buffer* const buffer, const char* const str, const int byte_count);

/**
 * Begin a list in the buffer.
 *
 * @param buffer The buffer to add to.
 * @return true if the operation was successful.
 */
bool cbe_start_list(cbe_buffer* const buffer);

/**
 * Begin a map in the buffer.
 *
 * @param buffer The buffer to add to.
 * @return true if the operation was successful.
 */
bool cbe_start_map(cbe_buffer* const buffer);

/**
 * End the current container in the buffer.
 *
 * @param buffer The buffer to add to.
 * @return true if the operation was successful.
 */
bool cbe_end_container(cbe_buffer* const buffer);

/**
 * Add a bitfield value to the buffer.
 * A bitfield is a packed array of bits, with low bits filled first.
 *
 * @param buffer The buffer to add to.
 * @param packed_values The values to add, pre-packed into bytes.
 * @param entity_count The number of bits to add.
 * @return true if the operation was successful.
 */
bool cbe_add_bitfield(cbe_buffer* const buffer, const uint8_t* const packed_values, const int entity_count);

/**
 * Add a bitfield to the buffer.
 *
 * @param buffer The buffer to add to.
 * @param value The values to add.
 * @param entity_count The number of bits to add.
 * @return true if the operation was successful.
 */
bool cbe_add_array_boolean(cbe_buffer* const buffer, const bool* const values, const int entity_count);

/**
 * Add an array of 8-bit integers to the buffer.
 *
 * @param buffer The buffer to add to.
 * @param value The values to add.
 * @param entity_count The number of values to add.
 * @return true if the operation was successful.
 */
bool cbe_add_array_int_8(cbe_buffer* const buffer, const int8_t* const values, const int entity_count);

/**
 * Add an array of 16-bit integers to the buffer.
 *
 * @param buffer The buffer to add to.
 * @param value The values to add.
 * @param entity_count The number of values to add.
 * @return true if the operation was successful.
 */
bool cbe_add_array_int_16(cbe_buffer* const buffer, const int16_t* const values, const int entity_count);

/**
 * Add an array of 32-bit integers to the buffer.
 *
 * @param buffer The buffer to add to.
 * @param value The values to add.
 * @param entity_count The number of values to add.
 * @return true if the operation was successful.
 */
bool cbe_add_array_int_32(cbe_buffer* const buffer, const int32_t* const values, const int entity_count);

/**
 * Add an array of 64-bit integers to the buffer.
 *
 * @param buffer The buffer to add to.
 * @param value The values to add.
 * @param entity_count The number of values to add.
 * @return true if the operation was successful.
 */
bool cbe_add_array_int_64(cbe_buffer* const buffer, const int64_t* const values, const int entity_count);

/**
 * Add an array of 128-bit integers to the buffer.
 *
 * @param buffer The buffer to add to.
 * @param value The values to add.
 * @param entity_count The number of values to add.
 * @return true if the operation was successful.
 */
bool cbe_add_array_int_128(cbe_buffer* const buffer, const __int128* const values, const int entity_count);

/**
 * Add an array of 32-bit floating point values to the buffer.
 *
 * @param buffer The buffer to add to.
 * @param value The values to add.
 * @param entity_count The number of values to add.
 * @return true if the operation was successful.
 */
bool cbe_add_array_float_32(cbe_buffer* const buffer, const float* const values, const int entity_count);

/**
 * Add an array of 64-bit floating point values to the buffer.
 *
 * @param buffer The buffer to add to.
 * @param value The values to add.
 * @param entity_count The number of values to add.
 * @return true if the operation was successful.
 */
bool cbe_add_array_float_64(cbe_buffer* const buffer, const double* const values, const int entity_count);

/**
 * Add an array of 128-bit floating point values to the buffer.
 *
 * @param buffer The buffer to add to.
 * @param value The values to add.
 * @param entity_count The number of values to add.
 * @return true if the operation was successful.
 */
bool cbe_add_array_float_128(cbe_buffer* const buffer, const __float128* const values, const int entity_count);

#if CBE_HAS_DECIMAL_SUPPORT
/**
 * Add a 32 bit decimal value to the buffer.
 * Note that this will add a narrower type if it will fit.
 *
 * @param buffer The buffer to add to.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_decimal_32(cbe_buffer* const buffer, const _Decimal32 value);

/**
 * Add a 64 bit decimal value to the buffer.
 * Note that this will add a narrower type if it will fit.
 *
 * @param buffer The buffer to add to.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_decimal_64(cbe_buffer* const buffer, const _Decimal64 value);

/**
 * Add a 128 bit decimal value to the buffer.
 * Note that this will add a narrower type if it will fit.
 *
 * @param buffer The buffer to add to.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_decimal_128(cbe_buffer* const buffer, const _Decimal128 value);

/**
 * Add an array of 32-bit decimal values to the buffer.
 *
 * @param buffer The buffer to add to.
 * @param value The values to add.
 * @param entity_count The number of values to add.
 * @return true if the operation was successful.
 */
bool cbe_add_array_decimal_32(cbe_buffer* const buffer, const _Decimal32* const values, const int entity_count);

/**
 * Add an array of 64-bit decimal values to the buffer.
 *
 * @param buffer The buffer to add to.
 * @param value The values to add.
 * @param entity_count The number of values to add.
 * @return true if the operation was successful.
 */
bool cbe_add_array_decimal_64(cbe_buffer* const buffer, const _Decimal64* const values, const int entity_count);

/**
 * Add an array of 64-bit decimal values to the buffer.
 *
 * @param buffer The buffer to add to.
 * @param value The values to add.
 * @param entity_count The number of values to add.
 * @return true if the operation was successful.
 */
bool cbe_add_array_decimal_128(cbe_buffer* const buffer, const _Decimal128* const values, const int entity_count);
#endif // CBE_HAS_DECIMAL_SUPPORT



// ------------
// Decoding API
// ------------

typedef enum
{
	CBE_NUMERIC_INT_8,
	CBE_NUMERIC_INT_16,
	CBE_NUMERIC_INT_32,
	CBE_NUMERIC_INT_64,
	CBE_NUMERIC_INT_128,
	CBE_NUMERIC_FLOAT_32,
	CBE_NUMERIC_FLOAT_64,
	CBE_NUMERIC_FLOAT_128,
	CBE_NUMERIC_DECIMAL_32,
	CBE_NUMERIC_DECIMAL_64,
	CBE_NUMERIC_DECIMAL_128,
} cbe_numeric_type;

/**
 * Union of all possible numeric types.
 * Used in on_number() to extract the real value by its correct type.
 */
typedef struct
{
	union
	{
		int8_t int_8;
		int16_t int_16;
		int32_t int_32;
		int64_t int_64;
		__int128 int_128;
		float float_32;
		double float_64;
		__float128 float_128;
#if CBE_HAS_DECIMAL_SUPPORT
		_Decimal32 decimal_32;
		_Decimal64 decimal_64;
		_Decimal128 decimal_128;
#endif // CBE_HAS_DECIMAL_SUPPORT
	} data;
	cbe_numeric_type type;
} cbe_number;

/**
 * Callback structure for use with cbe_decode().
 *
 * cbe_decode() will call these callbacks as it decodes encoded objects in the buffer.
 */
typedef struct {
	bool (*on_error)(const char* message);
	bool (*on_empty)(void);
	bool (*on_bool)(bool value);
	bool (*on_number)(cbe_number value);
	bool (*on_time)(cbe_time* value);
	bool (*on_end_container)(void);
	bool (*on_list_start)(void);
	bool (*on_map_start)(void);
	bool (*on_bitfield)(const uint8_t* start, const uint64_t bit_count);
	bool (*on_string)(const char* start, const char* end);
	bool (*on_array_int_8)(const int8_t* start, const int8_t* end);
	bool (*on_array_int_16)(const int16_t* start, const int16_t* end);
	bool (*on_array_int_32)(const int32_t* start, const int32_t* end);
	bool (*on_array_int_64)(const int64_t* start, const int64_t* end);
	bool (*on_array_int_128)(const __int128* start, const __int128* end);
	bool (*on_array_float_32)(const float* start, const float* end);
	bool (*on_array_float_64)(const double* start, const double* end);
	bool (*on_array_float_128)(const __float128* start, const __float128* end);
#if CBE_HAS_DECIMAL_SUPPORT
	bool (*on_array_decimal_32)(const _Decimal32* start, const _Decimal32* end);
	bool (*on_array_decimal_64)(const _Decimal64* start, const _Decimal64* end);
	bool (*on_array_decimal_128)(const _Decimal128* start, const _Decimal128* end);
#endif // CBE_HAS_DECIMAL_SUPPORT
	bool (*on_array_time)(const int64_t* start, const int64_t* end);
} cbe_decode_callbacks;


/**
 * Decode a buffer of encoded objects.
 *
 * @param callbacks The callbacks to call as it decodes objects.
 * @param data_start The start of the encoded buffer.
 * @param data_end The end (buffer + length) of the encoded buffer.
 */
const uint8_t* cbe_decode(cbe_decode_callbacks* callbacks, const uint8_t* const data_start, const uint8_t* const data_end);


#ifdef __cplusplus 
}
#endif
#endif // cbe_encoder_H
