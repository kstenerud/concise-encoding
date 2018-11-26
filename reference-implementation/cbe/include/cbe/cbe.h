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

typedef struct {} cbe_decode_process;

/**
 * Callback structure for use with cbe_decode().
 *
 * cbe_decode() will call these callbacks as it decodes objects in the document.
 */
typedef struct
{
	bool (*on_error)             (cbe_decode_process* decode_process, const char* message);
	bool (*on_empty)             (cbe_decode_process* decode_process);
	bool (*on_boolean)           (cbe_decode_process* decode_process, bool value);
	bool (*on_int_8)             (cbe_decode_process* decode_process, int8_t value);
	bool (*on_int_16)            (cbe_decode_process* decode_process, int16_t value);
	bool (*on_int_32)            (cbe_decode_process* decode_process, int32_t value);
	bool (*on_int_64)            (cbe_decode_process* decode_process, int64_t value);
	bool (*on_int_128)           (cbe_decode_process* decode_process, __int128 value);
	bool (*on_float_32)          (cbe_decode_process* decode_process, float value);
	bool (*on_float_64)          (cbe_decode_process* decode_process, double value);
	bool (*on_float_128)         (cbe_decode_process* decode_process, __float128 value);
	bool (*on_decimal_32)        (cbe_decode_process* decode_process, _Decimal32 value);
	bool (*on_decimal_64)        (cbe_decode_process* decode_process, _Decimal64 value);
	bool (*on_decimal_128)       (cbe_decode_process* decode_process, _Decimal128 value);
	bool (*on_time)              (cbe_decode_process* decode_process, int64_t value);
	bool (*on_end_container)     (cbe_decode_process* decode_process);
	bool (*on_list_start)        (cbe_decode_process* decode_process);
	bool (*on_map_start)         (cbe_decode_process* decode_process);
	bool (*on_bitfield)          (cbe_decode_process* decode_process, const uint8_t* start, const int64_t bit_count);
	bool (*on_string)            (cbe_decode_process* decode_process, const char* start, const int64_t byte_count);
	bool (*on_array)             (cbe_decode_process* decode_process, cbe_data_type type, const void* start, int64_t element_count);
} cbe_decode_callbacks;


/**
 * Begin a new decoding process.
 *
 * @param callbacks The callbacks to call while decoding the document.
 * @param user_context Whatever data you want to be available to the callbacks.
 * @return The process of the new decode process.
 */
cbe_decode_process* cbe_decode_begin(cbe_decode_callbacks* callbacks, void* user_context);

/**
 * Get the user context information from a decode process.
 * This is meant to be called by a decode callback function.
 *
 * @param encode_process The decode process.
 * @return The user context.
 */
void* cbe_decode_get_user_context(cbe_decode_process* decode_process);

/**
 * Decode part of a CBE document.
 * Returns false if it hasn't reached the end of the document (which means you must feed it more data).
 *
 * @param encode_process The decode process.
 * @param data_start The start of the document.
 * @param byte_count The number of bytes in the document fragment.
 * @return true if the document was fully decoded.
 */
bool cbe_decode_feed(cbe_decode_process* decode_process, const uint8_t* const data_start, const int64_t byte_count);

/**
 * Get the current write offset into the decode buffer.
 *
 * @param encode_process The decode process.
 * @return The current offset.
 */
int64_t cbe_decode_get_buffer_offset(cbe_decode_process* decode_process);

/**
 * End a decoding process, freeing up any resources used.
 * Note: This does NOT free the user context or callback structure.
 *
 * @param encode_process The decode process.
 */
void cbe_decode_end(cbe_decode_process* decode_process);



// ------------
// Encoding API
// ------------

typedef struct {} cbe_encode_process;

/**
 * Begin a new encoding process.
 *
 * @param document_buffer A buffer to store the document in.
 * @param byte_count Size of the buffer in bytes.
 * @return The process of the new encode process.
 */
cbe_encode_process* cbe_encode_begin(uint8_t* const document_buffer, int64_t byte_count);

/**
 * Replace the document buffer in an encode process.
 *
 * @param encode_process The encode process.
 * @param document_buffer A buffer to store the document in.
 * @param byte_count Size of the buffer in bytes.
 */
void cbe_encode_set_buffer(cbe_encode_process* encode_process, uint8_t* const document_buffer, int64_t byte_count);

/**
 * Get the current write offset into the encode buffer.
 *
 * @param encode_process The encode process.
 * @return The current offset.
 */
int64_t cbe_encode_get_buffer_offset(cbe_encode_process* encode_process);

/**
 * End an encoding process, freeing up any resources used.
 * Note: This does NOT free the user-supplied encode buffer.
 *
 * @param encode_process The encode process.
 */
void cbe_encode_end(cbe_encode_process* encode_process);

/**
 * Add an empty object to the document.
 *
 * @param encode_process The encode process.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_empty(cbe_encode_process* const encode_process);

/**
 * Add a boolean value to the document.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_boolean(cbe_encode_process* const encode_process, const bool value);

/**
 * Add an integer value to the document.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_int(cbe_encode_process* const encode_process, const int value);

/**
 * Add an integer value to the document.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_int_8(cbe_encode_process* const encode_process, const int8_t value);

/**
 * Add a 16 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_int_16(cbe_encode_process* const encode_process, const int16_t value);

/**
 * Add a 32 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_int_32(cbe_encode_process* const encode_process, const int32_t value);

/**
 * Add a 64 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_int_64(cbe_encode_process* const encode_process, const int64_t value);

/**
 * Add a 128 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_int_128(cbe_encode_process* const encode_process, const __int128 value);

/**
 * Add a 32 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_float_32(cbe_encode_process* const encode_process, const float value);

/**
 * Add a 64 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_float_64(cbe_encode_process* const encode_process, const double value);

/**
 * Add a 128 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_float_128(cbe_encode_process* const encode_process, const __float128 value);

/**
 * Add a 32 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_decimal_32(cbe_encode_process* const encode_process, const _Decimal32 value);

/**
 * Add a 64 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_decimal_64(cbe_encode_process* const encode_process, const _Decimal64 value);

/**
 * Add a 128 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_decimal_128(cbe_encode_process* const encode_process, const _Decimal128 value);

/**
 * Add a time value to the document.
 * Use cbe_new_time() to generate a time value.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_time(cbe_encode_process* const encode_process, const int64_t value);

/**
 * Add a UTF-8 encoded null-terminated string value to the document.
 * Do not include a byte order marker (BOM)
 *
 * @param encode_process The encode process.
 * @param str The null-terminated string to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_string(cbe_encode_process* const encode_process, const char* const str);

/**
 * Add a substring of a UTF-8 encoded string value to the document.
 * Do not include a byte order marker (BOM)
 *
 * @param encode_process The encode process.
 * @param string_start The start of the substring to add.
 * @param byte_count The length of the substring in bytes.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_substring(cbe_encode_process* const encode_process, const char* const string_start, const int64_t byte_count);

/**
 * Begin a list in the document.
 *
 * @param encode_process The encode process.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_start_list(cbe_encode_process* const encode_process);

/**
 * Begin a map in the document.
 *
 * @param encode_process The encode process.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_start_map(cbe_encode_process* const encode_process);

/**
 * End the current container in the document.
 *
 * @param encode_process The encode process.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_end_container(cbe_encode_process* const encode_process);

/**
 * Add a bitfield to the document.
 * A bitfield is a packed array of bits, with low bits filled first.
 *
 * @param encode_process The encode process.
 * @param packed_values The values to add, pre-packed into bytes.
 * @param element_count The number of bits to add.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_bitfield(cbe_encode_process* const encode_process, const uint8_t* const packed_values, const int64_t element_count);

/**
 * Add a bitfield to the document.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_boolean(cbe_encode_process* const encode_process, const bool* const elements, const int64_t element_count);

/**
 * Add an array of 8-bit integers to the document.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_int_8(cbe_encode_process* const encode_process, const int8_t* const elements, const int64_t element_count);

/**
 * Add an array of 16-bit integers to the document.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_int_16(cbe_encode_process* const encode_process, const int16_t* const elements, const int64_t element_count);

/**
 * Add an array of 32-bit integers to the document.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_int_32(cbe_encode_process* const encode_process, const int32_t* const elements, const int64_t element_count);

/**
 * Add an array of 64-bit integers to the document.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_int_64(cbe_encode_process* const encode_process, const int64_t* const elements, const int64_t element_count);

/**
 * Add an array of 128-bit integers to the document.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_int_128(cbe_encode_process* const encode_process, const __int128* const elements, const int64_t element_count);

/**
 * Add an array of 32-bit floating point values to the document.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_float_32(cbe_encode_process* const encode_process, const float* const elements, const int64_t element_count);

/**
 * Add an array of 64-bit floating point values to the document.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_float_64(cbe_encode_process* const encode_process, const double* const elements, const int64_t element_count);

/**
 * Add an array of 128-bit floating point values to the document.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_float_128(cbe_encode_process* const encode_process, const __float128* const elements, const int64_t element_count);

/**
 * Add an array of 32-bit decimal values to the document.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_decimal_32(cbe_encode_process* const encode_process, const _Decimal32* const elements, const int64_t element_count);

/**
 * Add an array of 64-bit decimal values to the document.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_decimal_64(cbe_encode_process* const encode_process, const _Decimal64* const elements, const int64_t element_count);

/**
 * Add an array of 64-bit decimal values to the document.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_decimal_128(cbe_encode_process* const encode_process, const _Decimal128* const elements, const int64_t element_count);

/**
 * Add an array of 64-bit time values to the document.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return true if there was enough room in the buffer to store the object.
 */
bool cbe_encode_add_array_time(cbe_encode_process* const encode_process, const int64_t* const elements, const int64_t element_count);



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
