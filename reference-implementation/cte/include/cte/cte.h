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
const char* cte_version();



// ------------
// Encoding API
// ------------

#define DEFAULT_INDENT_SPACES 0
#define DEFAULT_FLOAT_DIGITS_PRECISION 15

/**
 * Holds encoder contextual data.
 */
typedef struct
{
    const uint8_t* const start;
    const uint8_t* const end;
    uint8_t* pos;
    int indent_spaces;
    int float_digits_precision;
    int container_level;
    bool is_inside_map[200];
    bool is_first_in_document;
    bool is_first_in_container;
    bool next_object_is_map_key;
    // TODO: allowed_types
} cte_encode_context;


/**
 * Create a new encoding context metadata object.
 *
 * @param memory_start The start of a buffer to store the document in.
 * @param memory_end The end of the buffer.
 * @param indent_spaces The number of spaces to indent for pretty printing (0 = don't pretty print).
 * @param float_digits_precision The number of significant digits to print for floating point numbers.
 * @return The new context.
 */
cte_encode_context cte_new_encode_context_with_config(uint8_t* const memory_start,
                                                      uint8_t* const memory_end,
                                                      int indent_spaces,
                                                      int float_digits_precision);

/**
 * Create a new encoding context metadata object with the default configuration.
 *
 * @param memory_start The start of a buffer to store the document in.
 * @param memory_end The end of the buffer.
 * @return The new context.
 */
cte_encode_context cte_new_encode_context(uint8_t* const memory_start, uint8_t* const memory_end);

/**
 * Add an empty value to the document.
 *
 * @param context The encoding context.
 * @return true if the operation was successful.
 */
bool cte_add_empty(cte_encode_context* const context);

/**
 * Add a boolean value to the document.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cte_add_boolean(cte_encode_context* const context, const bool value);

/**
 * Add an integer value to the document.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cte_add_int(cte_encode_context* const context, const int value);

/**
 * Add an integer value to the document.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cte_add_int_8(cte_encode_context* const context, const int8_t value);

/**
 * Add a 16 bit integer value to the document.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cte_add_int_16(cte_encode_context* const context, const int16_t value);

/**
 * Add a 32 bit integer value to the document.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cte_add_int_32(cte_encode_context* const context, const int32_t value);

/**
 * Add a 64 bit integer value to the document.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cte_add_int_64(cte_encode_context* const context, const int64_t value);

/**
 * Add a 128 bit integer value to the document.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cte_add_int_128(cte_encode_context* const context, const __int128 value);

/**
 * Add a 32 bit floating point value to the document.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cte_add_float_32(cte_encode_context* const context, const float value);

/**
 * Add a 64 bit floating point value to the document.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cte_add_float_64(cte_encode_context* const context, const double value);

/**
 * Add a 128 bit floating point value to the document.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cte_add_float_128(cte_encode_context* const context, const __float128 value);

/**
 * Add a 32 bit decimal value to the document.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cte_add_decimal_32(cte_encode_context* const context, const _Decimal32 value);

/**
 * Add a 64 bit decimal value to the document.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cte_add_decimal_64(cte_encode_context* const context, const _Decimal64 value);

/**
 * Add a 128 bit decimal value to the document.
 *
 * @param context The encoding context.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cte_add_decimal_128(cte_encode_context* const context, const _Decimal128 value);

// TODO: cte_add_time

/**
 * Add a UTF-8 encoded null-terminated string value to the document.
 * Do not include a byte order marker (BOM)
 *
 * @param context The encoding context.
 * @param str The string to add.
 * @return true if the operation was successful.
 */
bool cte_add_string(cte_encode_context* const context, const char* const str);

/**
 * Add a substring of a UTF-8 encoded string value to the document.
 * Do not include a byte order marker (BOM)
 *
 * @param context The encoding context.
 * @param start The start of the substring to add.
 * @param end The end of the substring to add.
 * @return true if the operation was successful.
 */
bool cte_add_substring(cte_encode_context* const context, const char* const start, const char* const end);

/**
 * Begin a list in the context.
 *
 * @param context The encoding context.
 * @return true if the operation was successful.
 */
bool cte_start_list(cte_encode_context* const context);

/**
 * Begin a map in the context.
 *
 * @param context The encoding context.
 * @return true if the operation was successful.
 */
bool cte_start_map(cte_encode_context* const context);

/**
 * End the current container in the context.
 *
 * @param context The encoding context.
 * @return true if the operation was successful.
 */
bool cte_end_container(cte_encode_context* const context);

// TODO: cte_add_bitfield
// TODO: cte_add_array_xyz
// TODO: cte_start_array_xyz
// TODO: cte_end_array

/**
 * End the encoding process and ensure the encoded buffer is properly terminated.
 * Any opened lists or maps will be closed, and the encoded buffer will be null terminated.
 *
 * @param context The encoding context.
 * @return A pointer to the end of the encoded buffer. Returns NULL on error.
 */
const char* cte_end_encoding(cte_encode_context* const context);



// -----------
// Parsing API
// -----------

typedef struct
{
    void (*on_error)                   (void* context, const char* message);
    bool (*on_empty)                   (void* context);
    bool (*on_boolean)                 (void* context, bool value);
    bool (*on_int_8)                   (void* context, int8_t value);
    bool (*on_int_16)                  (void* context, int16_t value);
    bool (*on_int_32)                  (void* context, int32_t value);
    bool (*on_int_64)                  (void* context, int64_t value);
    bool (*on_int_128)                 (void* context, __int128 value);
    bool (*on_float_32)                (void* context, float value);
    bool (*on_float_64)                (void* context, double value);
    bool (*on_float_128)               (void* context, __float128 value);
    bool (*on_decimal_32)              (void* context, _Decimal32 value);
    bool (*on_decimal_64)              (void* context, _Decimal64 value);
    bool (*on_decimal_128)             (void* context, _Decimal128 value);
    // TODO: Parse and split time
    bool (*on_time)                    (void* context, const char* value);
    bool (*on_list_start)              (void* context);
    bool (*on_list_end)                (void* context);
    bool (*on_map_start)               (void* context);
    bool (*on_map_end)                 (void* context);
    bool (*on_string)                  (void* context, const char* value);
    bool (*on_array_start_boolean)     (void* context);
    bool (*on_array_start_int_8)       (void* context);
    bool (*on_array_start_int_16)      (void* context);
    bool (*on_array_start_int_32)      (void* context);
    bool (*on_array_start_int_64)      (void* context);
    bool (*on_array_start_int_128)     (void* context);
    bool (*on_array_start_float_32)    (void* context);
    bool (*on_array_start_float_64)    (void* context);
    bool (*on_array_start_float_128)   (void* context);
    bool (*on_array_start_decimal_32)  (void* context);
    bool (*on_array_start_decimal_64)  (void* context);
    bool (*on_array_start_decimal_128) (void* context);
    bool (*on_array_start_time)        (void* context);
    bool (*on_array_end)               (void* context);
} cte_parse_callbacks;

/**
 * Parse a CTE string.
 *
 * @param input The string to parse.
 * @param callbacks The callbacks to call as the parser encounters entities.
 * @param context Pointer to a user-supplied context object that gets passed directly to the callback functions.
 * @return true if parsing was successful.
 */
bool cte_parse_string(const char* input, const cte_parse_callbacks* callbacks, void* context);


#ifdef __cplusplus
}
#endif
