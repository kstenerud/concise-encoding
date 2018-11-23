#pragma once

#ifdef __cplusplus
// Use the same type names as C
#include <decimal/decimal>
typedef std::decimal::decimal32::__decfloat32   _Decimal32;
typedef std::decimal::decimal64::__decfloat64   _Decimal64;
typedef std::decimal::decimal128::__decfloat128 _Decimal128;
#endif

#ifdef __cplusplus 
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>


/**
 * Get the current library version.
 *
 * @return The library version.
 */
const char* cbe_version();



// ------------
// Encoding API
// ------------

/**
 * Holds encoder contextual data.
 */
typedef struct
{
	const uint8_t* const start;
	const uint8_t* const end;
	uint8_t* pos;
} cbe_encode_context;

/**
 * Create a new encoder context object.
 *
 * @param buffer_start The start of a buffer to store the document in.
 * @param buffer_end The end of the buffer.
 * @return The new context.
 */
cbe_encode_context cbe_new_encode_context(uint8_t* const buffer_start, uint8_t* const buffer_end);

/**
 * Create a new time value, encoding the fields into a 64-bit unsigned integer.
 * Note: This function does NOT validate input! Make sure your source values are correct!
 *
 * @param year The year (-131072 - 131071).
 * @param day The day of the year (1 - 366).
 * @param hour The hour of the day (0 - 23).
 * @param minute The minute of the hour (0 - 59).
 * @param second The second of the minute (0 - 60).
 * @param microsecond The microsecond of the second (0 - 999999).
 * @return A new time value.
 */
int64_t cbe_new_time(int year, int day, int hour, int minute, int second, int microsecond);

/**
 * Add an empty object to the document.
 *
 * @param context The encoding context.
 * @return true if the operation was successful.
 */
bool cbe_add_empty(cbe_encode_context* const context);

/**
 * Add a boolean value to the document.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_boolean(cbe_encode_context* const context, const bool value);

/**
 * Add an integer value to the document.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_int(cbe_encode_context* const context, const int value);

/**
 * Add an integer value to the document.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_int_8(cbe_encode_context* const context, const int8_t value);

/**
 * Add a 16 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_int_16(cbe_encode_context* const context, const int16_t value);

/**
 * Add a 32 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_int_32(cbe_encode_context* const context, const int32_t value);

/**
 * Add a 64 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_int_64(cbe_encode_context* const context, const int64_t value);

/**
 * Add a 128 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_int_128(cbe_encode_context* const context, const __int128 value);

/**
 * Add a 32 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_float_32(cbe_encode_context* const context, const float value);

/**
 * Add a 64 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_float_64(cbe_encode_context* const context, const double value);

/**
 * Add a 128 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_float_128(cbe_encode_context* const context, const __float128 value);

/**
 * Add a 32 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_decimal_32(cbe_encode_context* const context, const _Decimal32 value);

/**
 * Add a 64 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_decimal_64(cbe_encode_context* const context, const _Decimal64 value);

/**
 * Add a 128 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_decimal_128(cbe_encode_context* const context, const _Decimal128 value);

/**
 * Add a time value to the document.
 * Use cbe_new_time() to generate a time value.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cbe_add_time(cbe_encode_context* const context, const int64_t value);

/**
 * Add a UTF-8 encoded null-terminated string value to the document.
 * Do not include a byte order marker (BOM)
 *
 * @param context The encoding context.
 * @param str The string to add.
 * @return true if the operation was successful.
 */
bool cbe_add_string(cbe_encode_context* const context, const char* const str);

/**
 * Add a substring of a UTF-8 encoded string value to the document.
 * Do not include a byte order marker (BOM)
 *
 * @param context The encoding context.
 * @param start The start of the substring to add.
 * @param end The end of the substring to add.
 * @return true if the operation was successful.
 */
bool cbe_add_substring(cbe_encode_context* const context, const char* const start, const char* const end);

/**
 * Begin a list in the document.
 *
 * @param context The encoding context.
 * @return true if the operation was successful.
 */
bool cbe_start_list(cbe_encode_context* const context);

/**
 * Begin a map in the document.
 *
 * @param context The encoding context.
 * @return true if the operation was successful.
 */
bool cbe_start_map(cbe_encode_context* const context);

/**
 * End the current container in the document.
 *
 * @param context The encoding context.
 * @return true if the operation was successful.
 */
bool cbe_end_container(cbe_encode_context* const context);

/**
 * Add a bitfield to the document.
 * A bitfield is a packed array of bits, with low bits filled first.
 *
 * @param context The encoding context.
 * @param packed_values The values to add, pre-packed into bytes.
 * @param entity_count The number of bits to add.
 * @return true if the operation was successful.
 */
bool cbe_add_bitfield(cbe_encode_context* const context, const uint8_t* const packed_values, const int entity_count);

/**
 * Add a bitfield to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if the operation was successful.
 */
bool cbe_add_array_boolean(cbe_encode_context* const context, const bool* const start, const bool* const end);

/**
 * Add an array of 8-bit integers to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if the operation was successful.
 */
bool cbe_add_array_int_8(cbe_encode_context* const context, const int8_t* const start, const int8_t* const end);

/**
 * Add an array of 16-bit integers to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if the operation was successful.
 */
bool cbe_add_array_int_16(cbe_encode_context* const context, const int16_t* const start, const int16_t* const end);

/**
 * Add an array of 32-bit integers to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if the operation was successful.
 */
bool cbe_add_array_int_32(cbe_encode_context* const context, const int32_t* const start, const int32_t* const end);

/**
 * Add an array of 64-bit integers to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if the operation was successful.
 */
bool cbe_add_array_int_64(cbe_encode_context* const context, const int64_t* const start, const int64_t* const end);

/**
 * Add an array of 128-bit integers to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if the operation was successful.
 */
bool cbe_add_array_int_128(cbe_encode_context* const context, const __int128* const start, const __int128* const end);

/**
 * Add an array of 32-bit floating point values to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if the operation was successful.
 */
bool cbe_add_array_float_32(cbe_encode_context* const context, const float* const start, const float* const end);

/**
 * Add an array of 64-bit floating point values to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if the operation was successful.
 */
bool cbe_add_array_float_64(cbe_encode_context* const context, const double* const start, const double* const end);

/**
 * Add an array of 128-bit floating point values to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if the operation was successful.
 */
bool cbe_add_array_float_128(cbe_encode_context* const context, const __float128* const start, const __float128* const end);

/**
 * Add an array of 32-bit decimal values to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if the operation was successful.
 */
bool cbe_add_array_decimal_32(cbe_encode_context* const context, const _Decimal32* const start, const _Decimal32* const end);

/**
 * Add an array of 64-bit decimal values to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if the operation was successful.
 */
bool cbe_add_array_decimal_64(cbe_encode_context* const context, const _Decimal64* const start, const _Decimal64* const end);

/**
 * Add an array of 64-bit decimal values to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if the operation was successful.
 */
bool cbe_add_array_decimal_128(cbe_encode_context* const context, const _Decimal128* const start, const _Decimal128* const end);

// TODO: cbe_end_encoding? Needed?


// ------------
// Decoding API
// ------------

/**
 * Callback structure for use with cbe_decode().
 *
 * cbe_decode() will call these callbacks as it decodes objects in the document.
 */
typedef struct
{
	bool (*on_error)             (void* context, const char* message);
	bool (*on_empty)             (void* context);
	bool (*on_boolean)           (void* context, bool value);
	bool (*on_int_8)             (void* context, int8_t value);
	bool (*on_int_16)            (void* context, int16_t value);
	bool (*on_int_32)            (void* context, int32_t value);
	bool (*on_int_64)            (void* context, int64_t value);
	bool (*on_int_128)           (void* context, __int128 value);
	bool (*on_float_32)          (void* context, float value);
	bool (*on_float_64)          (void* context, double value);
	bool (*on_float_128)         (void* context, __float128 value);
	bool (*on_decimal_32)        (void* context, _Decimal32 value);
	bool (*on_decimal_64)        (void* context, _Decimal64 value);
	bool (*on_decimal_128)       (void* context, _Decimal128 value);
	bool (*on_time)              (void* context, int64_t value);
	bool (*on_end_container)     (void* context);
	bool (*on_list_start)        (void* context);
	bool (*on_map_start)         (void* context);
	bool (*on_bitfield)          (void* context, const uint8_t* start, const uint64_t bit_count);
	bool (*on_string)            (void* context, const char* start, const char* end);
	bool (*on_array_int_8)       (void* context, const int8_t* start, const int8_t* end);
	bool (*on_array_int_16)      (void* context, const int16_t* start, const int16_t* end);
	bool (*on_array_int_32)      (void* context, const int32_t* start, const int32_t* end);
	bool (*on_array_int_64)      (void* context, const int64_t* start, const int64_t* end);
	bool (*on_array_int_128)     (void* context, const __int128* start, const __int128* end);
	bool (*on_array_float_32)    (void* context, const float* start, const float* end);
	bool (*on_array_float_64)    (void* context, const double* start, const double* end);
	bool (*on_array_float_128)   (void* context, const __float128* start, const __float128* end);
	bool (*on_array_decimal_32)  (void* context, const _Decimal32* start, const _Decimal32* end);
	bool (*on_array_decimal_64)  (void* context, const _Decimal64* start, const _Decimal64* end);
	bool (*on_array_decimal_128) (void* context, const _Decimal128* start, const _Decimal128* end);
	bool (*on_array_time)        (void* context, const int64_t* start, const int64_t* end);
} cbe_decode_callbacks;


/**
 * Decode a CBE document.
 *
 * @param callbacks The callbacks to call as it decodes objects.
 * @param data_start The start of the document.
 * @param data_end The end (start + length) of the document.
 */
const uint8_t* cbe_decode(cbe_decode_callbacks* callbacks, const uint8_t* const data_start, const uint8_t* const data_end, void* context);

/**
 * Get the year component from a time value.
 *
 * @param time The time value.
 * @return the year component.
 */
int cbe_get_time_year(int64_t time);

/**
 * Get the day component from a time value.
 *
 * @param time The time value.
 * @return the day component.
 */
int cbe_get_time_day(int64_t time);

/**
 * Get the hour component from a time value.
 *
 * @param time The time value.
 * @return the hour component.
 */
int cbe_get_time_hour(int64_t time);

/**
 * Get the minute component from a time value.
 *
 * @param time The time value.
 * @return the minute component.
 */
int cbe_get_time_minute(int64_t time);

/**
 * Get the second component from a time value.
 *
 * @param time The time value.
 * @return the second component.
 */
int cbe_get_time_second(int64_t time);

/**
 * Get the microsecond component from a time value.
 *
 * @param time The time value.
 * @return the microsecond component.
 */
int cbe_get_time_microsecond(int64_t time);


#ifdef __cplusplus 
}
#endif
