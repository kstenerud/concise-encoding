#ifndef cte_H
#define cte_H
#ifdef __cplusplus
extern "C" {
#endif


#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
// C++ doesn't seem to have a POD implementation of decimal types.
    #define CTE_HAS_DECIMAL_SUPPORT 0
#else
    #define CTE_HAS_DECIMAL_SUPPORT 1
#endif

const char* cte_version();

typedef enum
{
    CTE_TYPE_BOOLEAN,
    CTE_TYPE_INTEGER,
    CTE_TYPE_FLOAT,
    CTE_TYPE_DECIMAL,
    CTE_TYPE_TIME,
} cte_type;

typedef struct
{
	void (*on_parse_error) (void* context, const char* message);
    void (*on_empty)       (void* context);
    void (*on_boolean)     (void* context, bool value);
    void (*on_int)         (void* context, int64_t value);
    void (*on_float)       (void* context, double value);
#if CTE_HAS_DECIMAL_SUPPORT
    void (*on_decimal)     (void* context, _Decimal64 value);
#endif
    void (*on_time)        (void* context, const char* value);
    void (*on_string)      (void* context, const char* value);
    void (*on_list_start)  (void* context);
    void (*on_list_end)    (void* context);
    void (*on_map_start)   (void* context);
    void (*on_map_end)     (void* context);
    void (*on_array_start) (void* context, cte_type type, int width);
    void (*on_array_end)   (void* context);
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
} cte_encode_context;


#define DEFAULT_INDENT_SPACES 0
#define DEFAULT_FLOAT_DIGITS_PRECISION 15

/**
 * Create a new encoding context metadata object.
 *
 * @param memory_start The start of the context's memory.
 * @param memory_end The end of the context's memory.
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
 * @param memory_start The start of the context's memory.
 * @param memory_end The end of the context's memory.
 * @return The new context.
 */
cte_encode_context cte_new_encode_context(uint8_t* const memory_start, uint8_t* const memory_end);

/**
 * Add an empty value to the context.
 *
 * @param context The context to add to.
 * @return true if the operation was successful.
 */
bool cte_add_empty(cte_encode_context* const context);

/**
 * Add a boolean value to the context.
 *
 * @param context The context to add to.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cte_add_boolean(cte_encode_context* const context, const bool value);

/**
 * Add an integer value to the context.
 *
 * @param context The context to add to.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cte_add_integer(cte_encode_context* const context, const int64_t value);

/**
 * Add a floating point value to the context.
 * Note that this will add a narrower type if it will fit.
 *
 * @param context The context to add to.
 * @param value The value to add.
 * @return true if the operation was successful.
 */
bool cte_add_float(cte_encode_context* const context, const double value);

/**
 * Add a UTF-8 encoded string value to the context.
 * Do not include a byte order marker (BOM)
 *
 * @param context The context to add to.
 * @param str The string to add.
 * @return true if the operation was successful.
 */
bool cte_add_string(cte_encode_context* const context, const char* const str);

/**
 * Add a UTF-8 encoded string value to the context.
 * Do not include a byte order marker (BOM)
 *
 * @param context The context to add to.
 * @param start The start of the substring to add.
 * @param end The end of the substring to add.
 * @return true if the operation was successful.
 */
bool cte_add_substring(cte_encode_context* const context, const char* const start, const char* const end);

/**
 * Begin a list in the context.
 *
 * @param context The context to add to.
 * @return true if the operation was successful.
 */
bool cte_start_list(cte_encode_context* const context);

/**
 * Begin a map in the context.
 *
 * @param context The context to add to.
 * @return true if the operation was successful.
 */
bool cte_start_map(cte_encode_context* const context);

/**
 * End the current container in the context.
 *
 * @param context The context to add to.
 * @return true if the operation was successful.
 */
bool cte_end_container(cte_encode_context* const context);

/**
 * End the encoding process and ensure the encoded buffer is properly terminated.
 * Any opened lists or maps will be closed, and the encoded buffer will be null terminated.
 *
 * @param context The encoding context.
 * @return A pointer to the end of the encoded buffer. Returns NULL on error.
 */
const char* cte_end_encoding(cte_encode_context* const context);


#ifdef __cplusplus
}
#endif
#endif // cte_H
