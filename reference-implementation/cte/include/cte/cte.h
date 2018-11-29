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
#include <smalltime/smalltime.h>

/**
 * Get the current library version as a semantic version (e.g. "1.5.2").
 *
 * @return The library version.
 */
const char* cte_version();



// ------------
// Decoding API
// ------------

typedef struct {} cte_decode_process;

/**
 * Status codes that can be returned by encoder functions.
 */
typedef enum
{
    /**
     * Returned when the document has been successfully decoded.
     */
    CTE_DECODE_STATUS_OK,

    /**
     * Returned when a callback returned false, stopping the decode process.
     * The process may be resumed after fixing whatever problem caused the
     * callback to return false.
     */
    CTE_DECODE_STATUS_STOPPED_IN_CALLBACK,

    /**
     * Returned when the decoder has reached the end of the buffer and needs
     * more data to finish decoding the document.
     */
    CTE_DECODE_STATUS_NEED_MORE_DATA,
} cte_decode_status;


typedef struct
{
    void (*on_error)                   (cte_decode_process* decode_process, const char* message);
    bool (*on_empty)                   (cte_decode_process* decode_process);
    bool (*on_boolean)                 (cte_decode_process* decode_process, bool value);
    bool (*on_int_8)                   (cte_decode_process* decode_process, int8_t value);
    bool (*on_int_16)                  (cte_decode_process* decode_process, int16_t value);
    bool (*on_int_32)                  (cte_decode_process* decode_process, int32_t value);
    bool (*on_int_64)                  (cte_decode_process* decode_process, int64_t value);
    bool (*on_int_128)                 (cte_decode_process* decode_process, __int128 value);
    bool (*on_float_32)                (cte_decode_process* decode_process, float value);
    bool (*on_float_64)                (cte_decode_process* decode_process, double value);
    bool (*on_float_128)               (cte_decode_process* decode_process, __float128 value);
    bool (*on_decimal_32)              (cte_decode_process* decode_process, _Decimal32 value);
    bool (*on_decimal_64)              (cte_decode_process* decode_process, _Decimal64 value);
    bool (*on_decimal_128)             (cte_decode_process* decode_process, _Decimal128 value);
    // TODO: Parse and split time
    bool (*on_time)                    (cte_decode_process* decode_process, const char* value);
    bool (*on_list_begin)              (cte_decode_process* decode_process);
    bool (*on_list_end)                (cte_decode_process* decode_process);
    bool (*on_map_begin)               (cte_decode_process* decode_process);
    bool (*on_map_end)                 (cte_decode_process* decode_process);
    bool (*on_string)                  (cte_decode_process* decode_process, const char* value);
    bool (*on_array_begin_boolean)     (cte_decode_process* decode_process);
    bool (*on_array_begin_int_8)       (cte_decode_process* decode_process);
    bool (*on_array_begin_int_16)      (cte_decode_process* decode_process);
    bool (*on_array_begin_int_32)      (cte_decode_process* decode_process);
    bool (*on_array_begin_int_64)      (cte_decode_process* decode_process);
    bool (*on_array_begin_int_128)     (cte_decode_process* decode_process);
    bool (*on_array_begin_float_32)    (cte_decode_process* decode_process);
    bool (*on_array_begin_float_64)    (cte_decode_process* decode_process);
    bool (*on_array_begin_float_128)   (cte_decode_process* decode_process);
    bool (*on_array_begin_decimal_32)  (cte_decode_process* decode_process);
    bool (*on_array_begin_decimal_64)  (cte_decode_process* decode_process);
    bool (*on_array_begin_decimal_128) (cte_decode_process* decode_process);
    bool (*on_array_begin_time)        (cte_decode_process* decode_process);
    bool (*on_array_end)               (cte_decode_process* decode_process);
} cte_decode_callbacks;


/**
 * Begin a new decoding process.
 *
 * @param callbacks The callbacks to call while decoding the document.
 * @param user_context Whatever data you want to be available to the callbacks.
 * @return The process of the new decode process.
 */
cte_decode_process* cte_decode_begin(cte_decode_callbacks* callbacks, void* user_context);

/**
 * Get the user context information from a decode process.
 * This is meant to be called by a decode callback function.
 *
 * @param encode_process The decode process.
 * @return The user context.
 */
void* cte_decode_get_user_context(cte_decode_process* decode_process);

/**
 * Decode part of a CBE document.
 *
 * Possible status codes:
 * - CTE_DECODE_STATUS_OK: the document has been completely decoded.
 * - CTE_DECODE_STATUS_NEED_MORE_DATA: out of data but not at end of document.
 * - CTE_DECODE_STATUS_STOPPED_IN_CALLBACK: a callback function returned false.
 *
 * @param encode_process The decode process.
 * @param data_start The start of the document.
 * @param byte_count The number of bytes in the document fragment.
 * @return The current decoder status.
 */
cte_decode_status cte_decode_feed(cte_decode_process* decode_process, const char* const data_start, const int64_t byte_count);

/**
 * End a decoding process, freeing up any decoder resources used.
 * Note: This does NOT free the user context or callback structure.
 *
 * @param encode_process The decode process.
 */
void cte_decode_end(cte_decode_process* decode_process);



// ------------
// Encoding API
// ------------

#define DEFAULT_INDENT_SPACES 0
#define DEFAULT_FLOAT_DIGITS_PRECISION 15

typedef struct {} cte_encode_process;

/**
 * Status codes that can be returned by encoder functions.
 */
typedef enum
{
    /**
     * Returned when a function completes successfully.
     */
    CTE_ENCODE_STATUS_OK,

    /**
     * Returned if the function would result in more container ends than
     * starts, or the document would be completed with containers still open.
     */
    CTE_ENCODE_STATUS_UNBALANCED_CONTAINERS,

    /**
     * Returned when attempting to add an invalid key type to a map.
     */
    CTE_ENCODE_STATUS_INCORRECT_KEY_TYPE,

    /**
     * Returned when terminating a map without adding a corresponding value
     * to the last key.
     */
    CTE_ENCODE_STATUS_MISSING_VALUE_FOR_KEY,

    /**
     * Returned when max container depth (default 500) is exceeded.
     */
    CTE_ENCODE_STATUS_MAX_CONTAINER_DEPTH_EXCEEDED,

    /**
     * Returned when the encoder has reached the end of the buffer and needs
     * more room to encode this object.
     */
    CTE_ENCODE_STATUS_NEED_MORE_ROOM,
} cte_encode_status;


/**
 * Begin a new encoding process.
 *
 * @param document_buffer A buffer to store the document in.
 * @param byte_count Size of the buffer in bytes.
 * @param indent_spaces The number of spaces to indent for pretty printing (0 = don't pretty print).
 * @param float_digits_precision The number of significant digits to print for floating point numbers.
 * @return The new encode process.
 */
cte_encode_process* cte_encode_begin_with_config(
                        uint8_t* const document_buffer,
                        int64_t byte_count,
                        int indent_spaces,
                        int float_digits_precision);

/**
 * Create a new encoding process with the default configuration.
 *
 * @param document_buffer A buffer to store the document in.
 * @param byte_count Size of the buffer in bytes.
 * @return The new encode process.
 */
cte_encode_process* cte_encode_begin(uint8_t* const document_buffer, int64_t byte_count);

/**
 * Replace the document buffer in an encode process.
 *
 * @param encode_process The encode process.
 * @param document_buffer A buffer to store the document in.
 * @param byte_count Size of the buffer in bytes.
 */
void cte_encode_set_buffer(cte_encode_process* encode_process, uint8_t* const document_buffer, int64_t byte_count);

/**
 * Get the current write offset into the encode buffer.
 *
 * @param encode_process The encode process.
 * @return The current offset.
 */
int64_t cte_encode_get_buffer_offset(cte_encode_process* encode_process);

/**
 * Get the document depth. This is the total depth of lists or maps that
 * haven't yet been terminated with an "end container". When you have
 * completed your document, it should be 0.
 *
 * A negative value indicates that you've added too many "end container"
 * objects, and the document is now invalid. Note: Adding more containers
 * to "balance it out" won't help; your code has a bug, and must be fixed.
 *
 * @param encode_process The encode process.
 * @return the document depth.
 */
int cte_encode_get_document_depth(cte_encode_process* encode_process);

/**
 * End an encoding process, freeing up any encoder resources used.
 * Note: This does NOT free the user-supplied encode buffer.
 * Note: Resources will be freed and process terminated EVEN ON ERROR.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_UNBALANCED_CONTAINERS: we're still in a container.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_end(cte_encode_process* const encode_process);

/**
 * Add an empty object to the document.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_STATUS_INCORRECT_KEY_TYPE: this can't be used as a map key.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_empty(cte_encode_process* const encode_process);

/**
 * Add a boolean value to the document.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_boolean(cte_encode_process* const encode_process, const bool value);

/**
 * Add an integer value to the document.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_int(cte_encode_process* const encode_process, const int value);

/**
 * Add an integer value to the document.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_int_8(cte_encode_process* const encode_process, const int8_t value);

/**
 * Add a 16 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_int_16(cte_encode_process* const encode_process, const int16_t value);

/**
 * Add a 32 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_int_32(cte_encode_process* const encode_process, const int32_t value);

/**
 * Add a 64 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_int_64(cte_encode_process* const encode_process, const int64_t value);

/**
 * Add a 128 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_int_128(cte_encode_process* const encode_process, const __int128 value);

/**
 * Add a 32 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_float_32(cte_encode_process* const encode_process, const float value);

/**
 * Add a 64 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_float_64(cte_encode_process* const encode_process, const double value);

/**
 * Add a 128 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_float_128(cte_encode_process* const encode_process, const __float128 value);

/**
 * Add a 32 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_decimal_32(cte_encode_process* const encode_process, const _Decimal32 value);

/**
 * Add a 64 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_decimal_64(cte_encode_process* const encode_process, const _Decimal64 value);

/**
 * Add a 128 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_decimal_128(cte_encode_process* const encode_process, const _Decimal128 value);

/**
 * Add a time value to the document.
 * Use cte_new_time() to generate a time value.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_time(cte_encode_process* const encode_process, const smalltime value);

/**
 * Add a UTF-8 encoded null-terminated string value to the document.
 * Do not include a byte order marker (BOM)
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param str The null-terminated string to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_string(cte_encode_process* const encode_process, const char* const str);

/**
 * Add a substring of a UTF-8 encoded string value to the document.
 * Do not include a byte order marker (BOM)
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param string_start The start of the substring to add.
 * @param byte_count The length of the substring in bytes.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_substring(cte_encode_process* const encode_process, const char* const string_start, const int64_t byte_count);

/**
 * Begin a list in the document. Must be matched by an end container.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_STATUS_INCORRECT_KEY_TYPE: this can't be used as a map key.
 * - CTE_ENCODE_STATUS_MAX_CONTAINER_DEPTH_EXCEEDED: container depth too deep.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_begin_list(cte_encode_process* const encode_process);

/**
 * Begin a map in the document. Must be matched by an end container.
 *
 * Map entries must be added in pairs. Every even item is a key, and every
 * odd item is a corresponding value.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_STATUS_INCORRECT_KEY_TYPE: this can't be used as a map key.
 * - CTE_ENCODE_STATUS_MAX_CONTAINER_DEPTH_EXCEEDED: container depth too deep.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_begin_map(cte_encode_process* const encode_process);

/**
 * End the current container (list or map) in the document.
 * If calling this function would result in too many end containers,
 * the operation is aborted and returns CTE_ENCODE_STATUS_UNBALANCED_CONTAINERS
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_STATUS_UNBALANCED_CONTAINERS: we're not in a container.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_end_container(cte_encode_process* const encode_process);

/**
 * Add a bitfield to the document.
 * A bitfield is a packed array of bits, with low bits filled first.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param packed_values The values to add, pre-packed into bytes.
 * @param element_count The number of bits to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_bitfield(cte_encode_process* const encode_process, const uint8_t* const packed_values, const int64_t element_count);

/**
 * Add a bitfield to the document.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_array_boolean(cte_encode_process* const encode_process, const bool* const elements, const int64_t element_count);

/**
 * Add an array of 8-bit integers to the document.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_array_int_8(cte_encode_process* const encode_process, const int8_t* const elements, const int64_t element_count);

/**
 * Add an array of 16-bit integers to the document.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_array_int_16(cte_encode_process* const encode_process, const int16_t* const elements, const int64_t element_count);

/**
 * Add an array of 32-bit integers to the document.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_array_int_32(cte_encode_process* const encode_process, const int32_t* const elements, const int64_t element_count);

/**
 * Add an array of 64-bit integers to the document.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_array_int_64(cte_encode_process* const encode_process, const int64_t* const elements, const int64_t element_count);

/**
 * Add an array of 128-bit integers to the document.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_array_int_128(cte_encode_process* const encode_process, const __int128* const elements, const int64_t element_count);

/**
 * Add an array of 32-bit floating point values to the document.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_array_float_32(cte_encode_process* const encode_process, const float* const elements, const int64_t element_count);

/**
 * Add an array of 64-bit floating point values to the document.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_array_float_64(cte_encode_process* const encode_process, const double* const elements, const int64_t element_count);

/**
 * Add an array of 128-bit floating point values to the document.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_array_float_128(cte_encode_process* const encode_process, const __float128* const elements, const int64_t element_count);

/**
 * Add an array of 32-bit decimal values to the document.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_array_decimal_32(cte_encode_process* const encode_process, const _Decimal32* const elements, const int64_t element_count);

/**
 * Add an array of 64-bit decimal values to the document.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_array_decimal_64(cte_encode_process* const encode_process, const _Decimal64* const elements, const int64_t element_count);

/**
 * Add an array of 64-bit decimal values to the document.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_array_decimal_128(cte_encode_process* const encode_process, const _Decimal128* const elements, const int64_t element_count);

/**
 * Add an array of 64-bit time values to the document.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param elements The array's elements.
 * @param element_count The number of elements in the array.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_array_time(cte_encode_process* const encode_process, const smalltime* const elements, const int64_t element_count);




#ifdef __cplusplus
}
#endif
