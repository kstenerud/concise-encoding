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

typedef enum
{
    CBE_TYPE_BOOLEAN,
    CBE_TYPE_INT_8,
    CBE_TYPE_INT_16,
    CBE_TYPE_INT_32,
    CBE_TYPE_INT_64,
    CBE_TYPE_INT_128,
    CBE_TYPE_FLOAT_32,
    CBE_TYPE_FLOAT_64,
    CBE_TYPE_FLOAT_128,
    CBE_TYPE_DECIMAL_32,
    CBE_TYPE_DECIMAL_64,
    CBE_TYPE_DECIMAL_128,
    CBE_TYPE_TIME,
} cbe_data_type;



/**
 * Get the current library version as a semantic version (e.g. "1.5.2").
 *
 * @return The library version.
 */
const char* cbe_version();



// ------------
// Decoding API
// ------------

typedef struct {} cbe_decode_context;

/**
 * Callback structure for use with cbe_decode().
 *
 * cbe_decode() will call these callbacks as it decodes objects in the document.
 */
typedef struct
{
	bool (*on_error)             (cbe_decode_context* context, const char* message);
	bool (*on_empty)             (cbe_decode_context* context);
	bool (*on_boolean)           (cbe_decode_context* context, bool value);
	bool (*on_int_8)             (cbe_decode_context* context, int8_t value);
	bool (*on_int_16)            (cbe_decode_context* context, int16_t value);
	bool (*on_int_32)            (cbe_decode_context* context, int32_t value);
	bool (*on_int_64)            (cbe_decode_context* context, int64_t value);
	bool (*on_int_128)           (cbe_decode_context* context, __int128 value);
	bool (*on_float_32)          (cbe_decode_context* context, float value);
	bool (*on_float_64)          (cbe_decode_context* context, double value);
	bool (*on_float_128)         (cbe_decode_context* context, __float128 value);
	bool (*on_decimal_32)        (cbe_decode_context* context, _Decimal32 value);
	bool (*on_decimal_64)        (cbe_decode_context* context, _Decimal64 value);
	bool (*on_decimal_128)       (cbe_decode_context* context, _Decimal128 value);
	bool (*on_time)              (cbe_decode_context* context, int64_t value);
	bool (*on_end_container)     (cbe_decode_context* context);
	bool (*on_list_start)        (cbe_decode_context* context);
	bool (*on_map_start)         (cbe_decode_context* context);
	bool (*on_bitfield)          (cbe_decode_context* context, const uint8_t* start, const uint64_t bit_count);
	bool (*on_string)            (cbe_decode_context* context, const char* start, const char* end);
	bool (*on_array)             (cbe_decode_context* context, cbe_data_type type, const void* start, uint64_t element_count);
} cbe_decode_callbacks;


/**
 * Begin a new decoding process.
 *
 * @param callbacks The callbacks to call while decoding the document.
 * @param user_context Whatever data you want to be available to the callbacks.
 * @return The context of the new decode process.
 */
cbe_decode_context* cbe_decode_begin(cbe_decode_callbacks* callbacks, void* user_context);

/**
 * Get the user context information from a decode context.
 * This is meant to be called by a decode callback function.
 *
 * @param context The decode context.
 * @return The user context.
 */
void* cbe_decode_get_user_context(cbe_decode_context* context);

/**
 * Decode part of a CBE document.
 * Returns false if it hasn't reached the end of the document (which means you must feed it more data).
 *
 * @param callbacks The callbacks to call as it decodes objects.
 * @param data_start The start of the document.
 * @param data_end The end (start + length) of the document.
 * @return true if the document was fully decoded.
 */
bool cbe_decode_feed(cbe_decode_context* context_ptr, const uint8_t* const data_start, const uint8_t* const data_end);

/**
 * Get the current write offset into the decode buffer.
 *
 * @param context The decoder context.
 * @return The current offset.
 */
int cbe_decode_get_buffer_offset(cbe_decode_context* context);

/**
 * End a decoding process, freeing up any resources used.
 * Note: This does NOT free the user context or callback structure.
 *
 * @param context The context of the decode process to end.
 */
void cbe_decode_end(cbe_decode_context* context);



// ------------
// Encoding API
// ------------

typedef struct {} cbe_encode_context;

/**
 * Begin a new encoding process.
 *
 * @param buffer_start The start of a buffer to store the document in.
 * @param buffer_end The end of the buffer.
 * @return The context of the new encode process.
 */
cbe_encode_context* cbe_encode_begin(uint8_t* const buffer_start, uint8_t* const buffer_end);

/**
 * Replace the document buffer in an encode process.
 *
 * @param context The encoder context.
 * @param buffer_start The start of a buffer to store the document in.
 * @param buffer_end The end of the buffer.
 */
void cbe_encode_set_buffer(cbe_encode_context* context, uint8_t* const buffer_start, uint8_t* const buffer_end);

/**
 * Get the current write offset into the encode buffer.
 *
 * @param context The encoder context.
 * @return The current offset.
 */
int cbe_encode_get_buffer_offset(cbe_encode_context* context);

/**
 * End an encoding process, freeing up any resources used.
 * Note: This does NOT free the user-supplied encode buffer.
 *
 * @param context The context of the encode process to end.
 */
void cbe_encode_end(cbe_encode_context* context);

/**
 * Add an empty object to the document.
 *
 * @param context The encoding context.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_empty(cbe_encode_context* const context);

/**
 * Add a boolean value to the document.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_boolean(cbe_encode_context* const context, const bool value);

/**
 * Add an integer value to the document.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_int(cbe_encode_context* const context, const int value);

/**
 * Add an integer value to the document.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_int_8(cbe_encode_context* const context, const int8_t value);

/**
 * Add a 16 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_int_16(cbe_encode_context* const context, const int16_t value);

/**
 * Add a 32 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_int_32(cbe_encode_context* const context, const int32_t value);

/**
 * Add a 64 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_int_64(cbe_encode_context* const context, const int64_t value);

/**
 * Add a 128 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_int_128(cbe_encode_context* const context, const __int128 value);

/**
 * Add a 32 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_float_32(cbe_encode_context* const context, const float value);

/**
 * Add a 64 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_float_64(cbe_encode_context* const context, const double value);

/**
 * Add a 128 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_float_128(cbe_encode_context* const context, const __float128 value);

/**
 * Add a 32 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_decimal_32(cbe_encode_context* const context, const _Decimal32 value);

/**
 * Add a 64 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_decimal_64(cbe_encode_context* const context, const _Decimal64 value);

/**
 * Add a 128 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_decimal_128(cbe_encode_context* const context, const _Decimal128 value);

/**
 * Add a time value to the document.
 * Use cbe_new_time() to generate a time value.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_time(cbe_encode_context* const context, const int64_t value);

/**
 * Add a UTF-8 encoded null-terminated string value to the document.
 * Do not include a byte order marker (BOM)
 *
 * @param context The encoding context.
 * @param str The string to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_string(cbe_encode_context* const context, const char* const str);

/**
 * Add a substring of a UTF-8 encoded string value to the document.
 * Do not include a byte order marker (BOM)
 *
 * @param context The encoding context.
 * @param start The start of the substring to add.
 * @param end The end of the substring to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_substring(cbe_encode_context* const context, const char* const start, const char* const end);

/**
 * Begin a list in the document.
 *
 * @param context The encoding context.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_start_list(cbe_encode_context* const context);

/**
 * Begin a map in the document.
 *
 * @param context The encoding context.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_start_map(cbe_encode_context* const context);

/**
 * End the current container in the document.
 *
 * @param context The encoding context.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_end_container(cbe_encode_context* const context);

/**
 * Add a bitfield to the document.
 * A bitfield is a packed array of bits, with low bits filled first.
 *
 * @param context The encoding context.
 * @param packed_values The values to add, pre-packed into bytes.
 * @param entity_count The number of bits to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_bitfield(cbe_encode_context* const context, const uint8_t* const packed_values, const int entity_count);

/**
 * Add a bitfield to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_boolean(cbe_encode_context* const context, const bool* const start, const bool* const end);

/**
 * Add an array of 8-bit integers to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_int_8(cbe_encode_context* const context, const int8_t* const start, const int8_t* const end);

/**
 * Add an array of 16-bit integers to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_int_16(cbe_encode_context* const context, const int16_t* const start, const int16_t* const end);

/**
 * Add an array of 32-bit integers to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_int_32(cbe_encode_context* const context, const int32_t* const start, const int32_t* const end);

/**
 * Add an array of 64-bit integers to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_int_64(cbe_encode_context* const context, const int64_t* const start, const int64_t* const end);

/**
 * Add an array of 128-bit integers to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_int_128(cbe_encode_context* const context, const __int128* const start, const __int128* const end);

/**
 * Add an array of 32-bit floating point values to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_float_32(cbe_encode_context* const context, const float* const start, const float* const end);

/**
 * Add an array of 64-bit floating point values to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_float_64(cbe_encode_context* const context, const double* const start, const double* const end);

/**
 * Add an array of 128-bit floating point values to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_float_128(cbe_encode_context* const context, const __float128* const start, const __float128* const end);

/**
 * Add an array of 32-bit decimal values to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_decimal_32(cbe_encode_context* const context, const _Decimal32* const start, const _Decimal32* const end);

/**
 * Add an array of 64-bit decimal values to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_decimal_64(cbe_encode_context* const context, const _Decimal64* const start, const _Decimal64* const end);

/**
 * Add an array of 64-bit decimal values to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_decimal_128(cbe_encode_context* const context, const _Decimal128* const start, const _Decimal128* const end);

/**
 * Add an array of 64-bit time values to the document.
 *
 * @param context The encoding context.
 * @param start The start of the array.
 * @param end The end of the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_time(cbe_encode_context* const context, const int64_t* const start, const int64_t* const end);



// --------
// Time API
// --------

/**
 * Create a new time value, encoding the fields into a 64-bit unsigned integer.
 * Note: This function does NOT validate input! Make sure your source values are correct!
 *
 * @param year The year (-131072 - 131071). Note: 1 = 1 AD, 0 = 1 BC, -1 = 2 BC, ...
 * @param day The day of the year, allowing for leap year (1 - 366).
 * @param hour The hour of the day (0 - 23).
 * @param minute The minute of the hour (0 - 59).
 * @param second The second of the minute, allowing for leap second (0 - 60).
 * @param microsecond The microsecond of the second (0 - 999999).
 * @return A new time value.
 */
int64_t cbe_new_time(int year, int day, int hour, int minute, int second, int microsecond);

/**
 * Get the year component from a time value.
 *
 * @param time The time value.
 * @return the year component.
 */
int cbe_time_get_year(int64_t time);

/**
 * Get the day component from a time value.
 *
 * @param time The time value.
 * @return the day component.
 */
int cbe_time_get_day(int64_t time);

/**
 * Get the hour component from a time value.
 *
 * @param time The time value.
 * @return the hour component.
 */
int cbe_time_get_hour(int64_t time);

/**
 * Get the minute component from a time value.
 *
 * @param time The time value.
 * @return the minute component.
 */
int cbe_time_get_minute(int64_t time);

/**
 * Get the second component from a time value.
 *
 * @param time The time value.
 * @return the second component.
 */
int cbe_time_get_second(int64_t time);

/**
 * Get the microsecond component from a time value.
 *
 * @param time The time value.
 * @return the microsecond component.
 */
int cbe_time_get_microsecond(int64_t time);


#ifdef __cplusplus 
}
#endif
