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

struct cte_decode_process;

/**
 * Status codes that can be returned by decoder functions.
 */
typedef enum
{
    /**
     * The document has been successfully decoded.
     */
    CTE_DECODE_STATUS_OK = 0,

    /**
     * Bison parser exited due to invalid input.
     * Matches return from yyabortlab.
     */
    CTE_DECODE_ERROR_PARSER_INVALID_INPUT = 1,

    /**
     * Bison parser ran out of memory.
     * Matches return from yyexhaustedlab.
     */
    CTE_DECODE_ERROR_PARSER_OUT_OF_MEMORY = 2,

    /**
     * The decoder has reached the end of the buffer and needs more data to
     * finish decoding the document.
     */
    CTE_DECODE_STATUS_NEED_MORE_DATA = 300,

    /**
     * A user callback returned false, stopping the decode process.
     * The process may be resumed after fixing whatever problem caused the
     * callback to return false.
     */
    CTE_DECODE_STATUS_STOPPED_IN_CALLBACK,

    /**
     * Unbalanced list/map begin and end markers were detected.
     */
    CTE_DECODE_ERROR_UNBALANCED_CONTAINERS,

    /**
     * An invalid data type was used as a map key.
     */
    CTE_DECODE_ERROR_INCORRECT_KEY_TYPE,

    /**
     * A map contained a key with no value.
     */
    CTE_DECODE_ERROR_MISSING_VALUE_FOR_KEY,

    /**
     * Bison parser failed to initialize.
     */
    CTE_DECODE_ERROR_PARSER_INITIALIZATION_ERROR,
} cte_decode_status;

/**
 * Callbacks for use with cte_decode(). These mirror the encode API.
 *
 * cte_decode() will call these callbacks as it decodes objects in the document.
 *
 * Callback functions return true if processing should continue.
 */
typedef struct
{
    // An error occurred
    void (*on_error) (struct cte_decode_process* decode_process, const char* message);

    // An empty field was decoded.
    bool (*on_empty) (struct cte_decode_process* decode_process);

    // An boolean field was decoded.
    bool (*on_boolean) (struct cte_decode_process* decode_process, bool value);

    // An 8-bit integer field was decoded.
    bool (*on_int_8) (struct cte_decode_process* decode_process, int8_t value);

    // A 16-bit integer field was decoded.
    bool (*on_int_16) (struct cte_decode_process* decode_process, int16_t value);

    // A 32-bit integer field was decoded.
    bool (*on_int_32) (struct cte_decode_process* decode_process, int32_t value);

    // A 64-bit integer field was decoded.
    bool (*on_int_64) (struct cte_decode_process* decode_process, int64_t value);

    // A 128-bit integer field was decoded.
    bool (*on_int_128) (struct cte_decode_process* decode_process, __int128 value);

    // A 32-bit binary floating point field was decoded.
    bool (*on_float_32) (struct cte_decode_process* decode_process, float value);

    // A 64-bit binary floating point field was decoded.
    bool (*on_float_64) (struct cte_decode_process* decode_process, double value);

    // A 128-bit binary floating point field was decoded.
    bool (*on_float_128) (struct cte_decode_process* decode_process, __float128 value);

    // A 32-bit decimal floating point field was decoded.
    bool (*on_decimal_32) (struct cte_decode_process* decode_process, _Decimal32 value);

    // A 64-bit decimal floating point field was decoded.
    bool (*on_decimal_64) (struct cte_decode_process* decode_process, _Decimal64 value);

    // A 128-bit decimal floating point field was decoded.
    bool (*on_decimal_128) (struct cte_decode_process* decode_process, _Decimal128 value);

    // A time field was decoded.
    bool (*on_time) (struct cte_decode_process* decode_process, const char* value);

    // A list container has been opened.
    bool (*on_list_begin) (struct cte_decode_process* decode_process);

    // A list container has been closed.
    bool (*on_list_end) (struct cte_decode_process* decode_process);

    // A map container has been opened.
    bool (*on_map_begin) (struct cte_decode_process* decode_process);

    // A map container has been closed.
    bool (*on_map_end) (struct cte_decode_process* decode_process);

    // TODO: How to split thsse up?
    bool (*on_string) (struct cte_decode_process* decode_process, const char* value);

    bool (*on_binary_data_begin) (struct cte_decode_process* decode_process);

    bool (*on_binary_data_end) (struct cte_decode_process* decode_process);
} cte_decode_callbacks;


/**
 * Get the size of the decode process data.
 * Use this to create a backing store for the process data like so:
 *     char process_backing_store[cte_decode_process_size()];
 *     struct cte_decode_process* decode_process = (struct cte_decode_process*)process_backing_store;
 * or
 *     struct cte_decode_process* decode_process = (struct cte_decode_process*)malloc(cte_decode_process_size());
 * or
 *     std::vector<char> process_backing_store(cte_decode_process_size());
 *     struct cte_decode_process* decode_process = (struct cte_decode_process*)process_backing_store.data();
 *
 * @return The process data size.
 */
int cte_decode_process_size();

/**
 * Begin a new decoding process.
 *
 * Successful status codes:
 * - CTE_DECODE_STATUS_OK: The decode process has begun.
 *
 * Unrecoverable codes:
 * - CTE_DECODE_ERROR_PARSER_INITIALIZATION_ERROR
 *
 * @param decode_process The decode process to initialize.
 * @param callbacks The callbacks to call while decoding the document.
 * @param user_context Whatever data you want to be available to the callbacks.
 * @return The current decoder status.
 */
cte_decode_status cte_decode_begin(struct cte_decode_process* decode_process,
                                   const cte_decode_callbacks* callbacks,
                                   void* user_context);

/**
 * Get the user context information from a decode process.
 * This is meant to be called by a decode callback function.
 *
 * @param decode_process The decode process.
 * @return The user context.
 */
void* cte_decode_get_user_context(struct cte_decode_process* decode_process);

/**
 * Decode part of a CTE document.
 *
 * Note: data_start is not const because 
 *
 * Successful status codes:
 * - CTE_DECODE_STATUS_OK: document has been completely decoded.
 * - CTE_DECODE_STATUS_NEED_MORE_DATA: out of data but not at end of document.
 *
 * Unrecoverable codes:
 * - CTE_DECODE_ERROR_UNBALANCED_CONTAINERS: a map or list is missing an end marker.
 * - CTE_DECODE_ERROR_INCORRECT_KEY_TYPE: document has an invalid key type.
 * - CTE_DECODE_ERROR_MISSING_VALUE_FOR_KEY: document has a map key with no value.
 *
 * Recoverable codes:
 * - CTE_DECODE_STATUS_STOPPED_IN_CALLBACK: a callback function returned false.
 *
 * Encountering CTE_DECODE_STATUS_NEED_MORE_DATA means that cte_decode_feed()
 * has decoded everything it can from the current buffer, and is ready to
 * receive the next buffer of encoded data. It is important to get the
 * current buffer offset after receiving this status code, because some bytes
 * at the end of the current buffer may not have been consumed; these
 * unconsumed bytes must be prepended to the subsequent buffer for the next
 * call to cte_decode_feed().
 *
 * If an unrecoverable code is returned, the document is invalid, and must be
 * discarded.
 *
 * If a recoverable code is returned, the decoder process points to the field
 * that caused the code to be returned. If you can fix the problem that led to
 * the recoverable code, you may run `cte_decode_feed()` again using the same
 * buffer + the current offset from `cte_decode_get_buffer_offset()`.
 *
 * @param decode_process The decode process.
 * @param data_start The start of the document.
 * @param byte_count The number of bytes in the document fragment.
 * @return The current decoder status.
 */
cte_decode_status cte_decode_feed(struct cte_decode_process* decode_process,
                                  const char* data_start,
                                  int64_t byte_count);

/**
 * End a decoding process, checking for document validity.
 *
 * Possible error codes:
 * - CTE_DECODE_ERROR_UNBALANCED_CONTAINERS: one or more containers were not closed.
 * - CTE_DECODE_ERROR_INCOMPLETE_FIELD: a field has not been completely filled yet.
 *
 * @param decode_process The decode process.
 * @return The final decoder status.
 */
cte_decode_status cte_decode_end(struct cte_decode_process* decode_process);



// ------------
// Encoding API
// ------------

#define DEFAULT_INDENT_SPACES 0
#define DEFAULT_FLOAT_DIGITS_PRECISION 15

struct cte_encode_process;

/**
 * Status codes that can be returned by encoder functions.
 */
typedef enum
{
    /**
     * Completed successfully.
     */
    CTE_ENCODE_STATUS_OK = 0,

    /**
     * The encoder has reached the end of the buffer and needs more room to
     * encode this object.
     */
    CTE_ENCODE_STATUS_NEED_MORE_ROOM = 400,

    /**
     * Unbalanced list/map begin and end markers were detected.
     */
    CTE_ENCODE_ERROR_UNBALANCED_CONTAINERS,

    /**
     * An invalid data type was used as a map key.
     */
    CTE_ENCODE_ERROR_INCORRECT_KEY_TYPE,

    /**
     * A map contained a key with no value.
     */
    CTE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY,

    /**
     * Attempted to add a new field or close the document before the existing
     * field was completely filled.
     */
    CTE_ENCODE_ERROR_INCOMPLETE_FIELD,

    /**
     * The currently open field is smaller than the data being added.
     */
    CTE_ENCODE_ERROR_FIELD_LENGTH_EXCEEDED,

    /**
     * We're not inside an array field.
     */
    CTE_ENCODE_ERROR_NOT_INSIDE_ARRAY_FIELD,

    /**
     * Max container depth (default 500) was exceeded.
     */
    CTE_ENCODE_ERROR_MAX_CONTAINER_DEPTH_EXCEEDED,
} cte_encode_status;


/**
 * Get the size of the encode process data.
 * Use this to create a backing store for the process data like so:
 *     char process_backing_store[cte_encode_process_size()];
 *     struct cte_encode_process* encode_process = (struct cte_encode_process*)process_backing_store;
 * or
 *     struct cte_encode_process* encode_process = (struct cte_encode_process*)malloc(cte_encode_process_size());
 * or
 *     std::vector<char> process_backing_store(cte_encode_process_size());
 *     struct cte_encode_process* encode_process = (struct cte_encode_process*)process_backing_store.data();
 *
 * @return The process data size.
 */
int cte_encode_process_size();

/**
 * Begin a new encoding process.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * @param document_buffer A buffer to store the document in.
 * @param byte_count Size of the buffer in bytes.
 * @param indent_spaces The number of spaces to indent for pretty printing (0 = don't pretty print).
 * @param float_digits_precision The number of significant digits to print for floating point numbers.
 * @return The new encode process.
 */
cte_encode_status cte_encode_begin_with_config(
                        struct cte_encode_process* encode_process,
                        uint8_t* const document_buffer,
                        int64_t byte_count,
                        int indent_spaces,
                        int float_digits_precision);

/**
 * Begin a new encoding process with the default configuration.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * @param document_buffer A buffer to store the document in.
 * @param byte_count Size of the buffer in bytes.
 * @return The new encode process.
 */
cte_encode_status cte_encode_begin(
                        struct cte_encode_process* encode_process,
                        uint8_t* const document_buffer,
                        int64_t byte_count);

/**
 * Replace the document buffer in an encode process.
 * This also resets the buffer offset.
 *
 * @param encode_process The encode process.
 * @param document_buffer A buffer to store the document in.
 * @param byte_count Size of the buffer in bytes.
 */
void cte_encode_set_buffer(struct cte_encode_process* encode_process, uint8_t* const document_buffer, int64_t byte_count);

/**
 * Get the current write offset into the encode buffer.
 * This points to one past the last byte written to the current buffer.
 *
 * @param encode_process The encode process.
 * @return The current offset.
 */
int64_t cte_encode_get_buffer_offset(struct cte_encode_process* encode_process);

/**
 * Get the document depth. This is the total depth of lists or maps that
 * haven't yet been terminated with an "end container". It must be 0 in order
 * to successfully complete a document.
 *
 * A negative value indicates that you've added too many "end container"
 * objects, and the document is now invalid. Note: Adding more containers
 * to "balance it out" won't help; your code has a bug, and must be fixed.
 *
 * @param encode_process The encode process.
 * @return the document depth.
 */
int cte_encode_get_document_depth(struct cte_encode_process* encode_process);

/**
 * End an encoding process, checking the document for validity.
 *
 * Possible error codes:
 * - CTE_ENCODE_ERROR_UNBALANCED_CONTAINERS: one or more containers were not closed.
 * - CTE_ENCODE_ERROR_INCOMPLETE_FIELD: a field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @return The final encoder status.
 */
cte_encode_status cte_encode_end(struct cte_encode_process* encode_process);

/**
 * Add an empty object to the document.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_ERROR_INCORRECT_KEY_TYPE: this can't be used as a map key.
 * - CTE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_empty(struct cte_encode_process* encode_process);

/**
 * Add a boolean value to the document.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_boolean(struct cte_encode_process* encode_process, bool value);

/**
 * Add an integer value to the document.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_int(struct cte_encode_process* encode_process, int value);

/**
 * Add an integer value to the document.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_int_8(struct cte_encode_process* encode_process, int8_t value);

/**
 * Add a 16 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_int_16(struct cte_encode_process* encode_process, int16_t value);

/**
 * Add a 32 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_int_32(struct cte_encode_process* encode_process, int32_t value);

/**
 * Add a 64 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_int_64(struct cte_encode_process* encode_process, int64_t value);

/**
 * Add a 128 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_int_128(struct cte_encode_process* encode_process, __int128 value);

/**
 * Add a 32 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_float_32(struct cte_encode_process* encode_process, float value);

/**
 * Add a 64 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_float_64(struct cte_encode_process* encode_process, double value);

/**
 * Add a 128 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_float_128(struct cte_encode_process* encode_process, __float128 value);

/**
 * Add a 32 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_decimal_32(struct cte_encode_process* encode_process, _Decimal32 value);

/**
 * Add a 64 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_decimal_64(struct cte_encode_process* encode_process, _Decimal64 value);

/**
 * Add a 128 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_decimal_128(struct cte_encode_process* encode_process, _Decimal128 value);

/**
 * Add a time value to the document.
 * Use cte_new_time() to generate a time value.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_time(struct cte_encode_process* encode_process, smalltime value);

/**
 * Add a UTF-8 encoded string and its data to a document.
 * Note: Do not include a byte order marker (BOM).
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param str The string to add.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_string(struct cte_encode_process* encode_process, const char* str);

/**
 * Add a substring of a UTF-8 encoded string value to the document.
 * Do not include a byte order marker (BOM)
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param string_start The start of the substring to add.
 * @param byte_count The length of the substring in bytes.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_substring(struct cte_encode_process* encode_process, const char* string_start, int64_t byte_count);

/**
 * Begin a list in the document. Must be matched by an end container.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_ERROR_INCORRECT_KEY_TYPE: this can't be used as a map key.
 * - CTE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 * - CTE_ENCODE_ERROR_MAX_CONTAINER_DEPTH_EXCEEDED: container depth too deep.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_begin_list(struct cte_encode_process* encode_process);

/**
 * Begin a map in the document. Must be matched by an end container.
 *
 * Map entries must be added in pairs. Every even item is a key, and every
 * odd item is a corresponding value.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_ERROR_INCORRECT_KEY_TYPE: this can't be used as a map key.
 * - CTE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 * - CTE_ENCODE_ERROR_MAX_CONTAINER_DEPTH_EXCEEDED: container depth too deep.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_begin_map(struct cte_encode_process* encode_process);

/**
 * End the current container (list or map) in the document.
 * If calling this function would result in too many end containers,
 * the operation is aborted and returns CTE_ENCODE_ERROR_UNBALANCED_CONTAINERS
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CTE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 * - CTE_ENCODE_ERROR_UNBALANCED_CONTAINERS: we're not in a container.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_end_container(struct cte_encode_process* encode_process);

/**
* TODO: Streaming
 * Add an array of binary data to the document.
 *
 * Possible error codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param start The start of the data.
 * @param byte_count The length of the data in bytes.
 * @return The current encoder status.
 */
cte_encode_status cte_encode_add_binary_data(struct cte_encode_process* encode_process, const uint8_t* start, int64_t byte_count);



#ifdef __cplusplus
}
#endif
