#pragma once

#ifndef CTE_PUBLIC
    #if defined _WIN32 || defined __CYGWIN__
        #define CTE_PUBLIC __declspec(dllimport)
    #else
        #define CTE_PUBLIC
    #endif
#endif

#ifndef ANSI_EXTENSION
    #ifdef __GNUC__
        #define ANSI_EXTENSION __extension__
    #else
        #define ANSI_EXTENSION
    #endif
#endif

#ifdef __cplusplus
// Use the same type names as C
#include <decimal/decimal>
ANSI_EXTENSION typedef std::decimal::decimal32::__decfloat32   dec32_ct;
ANSI_EXTENSION typedef std::decimal::decimal64::__decfloat64   dec64_ct;
ANSI_EXTENSION typedef std::decimal::decimal128::__decfloat128 dec128_ct;
#else
    ANSI_EXTENSION typedef _Decimal32 dec32_ct;
    ANSI_EXTENSION typedef _Decimal64 dec64_ct;
    ANSI_EXTENSION typedef _Decimal128 dec128_ct;
#endif
ANSI_EXTENSION typedef __int128 int128_ct;
ANSI_EXTENSION typedef __float128 float128_ct;

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <smalltime/smalltime.h>



// ========
// Defaults
// ========

#ifndef CTE_DEFAULT_MAX_CONTAINER_DEPTH
    #define CTE_DEFAULT_MAX_CONTAINER_DEPTH 500
#endif
#ifndef CTE_DEFAULT_INDENT_SPACES
    #define CTE_DEFAULT_INDENT_SPACES 4
#endif



// -----------
// Library API
// -----------

/**
 * Get the current library version as a semantic version (e.g. "1.5.2").
 *
 * @return The library version.
 */
CTE_PUBLIC const char* cte_version();



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
     * One or more of the arguments was invalid.
     */
    CTE_DECODE_ERROR_INVALID_ARGUMENT,

    /**
     * The array data was invalid.
     */
    CTE_DECODE_ERROR_INVALID_ARRAY_DATA,

    /**
     * Unbalanced list/map begin and end markers were detected.
     */
    CTE_DECODE_ERROR_UNBALANCED_CONTAINERS,

    /**
     * An invalid data type was used as a map key.
     */
    CTE_DECODE_ERROR_INCORRECT_MAP_KEY_TYPE,

    /**
     * A map contained a key with no value.
     */
    CTE_DECODE_ERROR_MAP_MISSING_VALUE_FOR_KEY,

    /**
     * An array field was not completed before ending the decode process.
     */
    CTE_DECODE_ERROR_INCOMPLETE_ARRAY_FIELD,

    /**
     * Max container depth (default 500) was exceeded.
     */
    CTE_DECODE_ERROR_MAX_CONTAINER_DEPTH_EXCEEDED,

    /**
     */
    CTE_DECODE_ERROR_PARSE_UNEXPECTED_CHARACTER,

    /**
     * An internal bug triggered an error.
     */
    CTE_DECODE_ERROR_INTERNAL_BUG,
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
    // A nil field was decoded.
    bool (*on_nil) (struct cte_decode_process* decode_process);

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
    bool (*on_int_128) (struct cte_decode_process* decode_process, int128_ct value);

    // A 32-bit binary floating point field was decoded.
    bool (*on_float_32) (struct cte_decode_process* decode_process, float value);

    // A 64-bit binary floating point field was decoded.
    bool (*on_float_64) (struct cte_decode_process* decode_process, double value);

    // A 128-bit binary floating point field was decoded.
    bool (*on_float_128) (struct cte_decode_process* decode_process, float128_ct value);

    // A 32-bit decimal floating point field was decoded.
    bool (*on_decimal_32) (struct cte_decode_process* decode_process, dec32_ct value);

    // A 64-bit decimal floating point field was decoded.
    bool (*on_decimal_64) (struct cte_decode_process* decode_process, dec64_ct value);

    // A 128-bit decimal floating point field was decoded.
    bool (*on_decimal_128) (struct cte_decode_process* decode_process, dec128_ct value);

    // A time field was decoded.
    bool (*on_time) (struct cte_decode_process* decode_process, smalltime value);

    // A list has been opened.
    bool (*on_list_begin) (struct cte_decode_process* decode_process);

    // The current list has been closed.
    bool (*on_list_end) (struct cte_decode_process* decode_process);

    // A map has been opened.
    bool (*on_map_begin) (struct cte_decode_process* decode_process);

    // The current map has been closed.
    bool (*on_map_end) (struct cte_decode_process* decode_process);

    /**
     * A string has been opened. Expect subsequent calls to
     * on_string_data() until the array has been filled. Once
     * on_string_end() is called, the field is considered
     * "complete" and is closed.
     *
     * @param decode_process The decode process.
     */
    bool (*on_string_begin) (struct cte_decode_process* decode_process);

    /**
     * String data was decoded, and should be added to the current string field.
     *
     * @param decode_process The decode process.
     * @param start The start of the data.
     * @param byte_count The number of bytes in this array fragment.
     */
    bool (*on_string_data) (struct cte_decode_process* decode_process,
                            const char* start,
                            int64_t byte_count);

    /**
     * The current string has been closed.
     *
     * @param decode_process The decode process.
     */
    bool (*on_string_end) (struct cte_decode_process* decode_process);

    /**
     * A binary data array has been opened. Expect subsequent calls to
     * on_binary_data() until the array has been filled. Once
     * on_binary_end() is called, the field is considered
     * "complete" and is closed.
     *
     * @param byte_count The total length of the array.
     */
    bool (*on_binary_begin) (struct cte_decode_process* decode_process);

    /**
     * Binary data was decoded, and should be added to the current binary field.
     *
     * @param decode_process The decode process.
     * @param start The start of the data.
     * @param byte_count The number of bytes in this array fragment.
     */
    bool (*on_binary_data) (struct cte_decode_process* decode_process,
                            const uint8_t* start,
                            int64_t byte_count);

    /**
     * The current binary array has been closed.
     *
     * @param decode_process The decode process.
     */
    bool (*on_binary_end) (struct cte_decode_process* decode_process);

    /**
     * A comment has been opened. Expect subsequent calls to
     * on_comment_data() until the array has been filled. Once
     * on_comment_end() is called, the field is considered
     * "complete" and is closed.
     *
     * @param decode_process The decode process.
     */
    bool (*on_comment_begin) (struct cte_decode_process* decode_process);

    /**
     * String data was decoded, and should be added to the current comment field.
     *
     * @param decode_process The decode process.
     * @param start The start of the data.
     * @param byte_count The number of bytes in this array fragment.
     */
    bool (*on_comment_data) (struct cte_decode_process* decode_process,
                             const char* start,
                             int64_t byte_count);

    /**
     * The current comment has been closed.
     *
     * @param decode_process The decode process.
     */
    bool (*on_comment_end) (struct cte_decode_process* decode_process);
} cte_decode_callbacks;


// ------------------
// Decoder Simple API
// ------------------

/**
 * Get the user context information from a decode process.
 * This is meant to be called by a decode callback function.
 *
 * @param decode_process The decode process.
 * @return The user context.
 */
CTE_PUBLIC void* cte_decode_get_user_context(struct cte_decode_process* decode_process);

/**
 * Decode an entire CTE document.
 *
 * Successful status codes:
 * - CTE_DECODE_STATUS_OK: document has been completely decoded.
 *
 * Unrecoverable codes:
 * - CTE_DECODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_DECODE_ERROR_INVALID_ARRAY_DATA: An array type contained invalid data.
 * - CTE_DECODE_ERROR_UNBALANCED_CONTAINERS: a map or list is missing an end marker.
 * - CTE_DECODE_ERROR_INCORRECT_MAP_KEY_TYPE: a map has an invalid key type.
 * - CTE_DECODE_ERROR_MAP_MISSING_VALUE_FOR_KEY: document has a map key with no value.
 * - CTE_DECODE_ERROR_INCOMPLETE_ARRAY_FIELD: An array was not completed before document end.
 * - CTE_DECODE_ERROR_MAX_CONTAINER_DEPTH_EXCEEDED: Containers run too deep in the document.
 * - CTE_DECODE_STATUS_STOPPED_IN_CALLBACK: a callback function returned false.
 *
 * @param callbacks The callbacks to call while decoding the document.
 * @param user_context Whatever data you want to be available to the callbacks.
 * @param document_start The start of the document.
 * @param byte_count The number of bytes in the document.
 * @param max_container_depth The maximum container depth to suppport (<=0 means use default).
 */
CTE_PUBLIC cte_decode_status cte_decode(const cte_decode_callbacks* callbacks,
                                        void* user_context,
                                        const char* document_start,
                                        int64_t byte_count,
                                        int max_container_depth);



// -----------------
// Decoder Power API
// -----------------

/**
 * Get the size of the decode process data.
 * Use this to create a backing store for the process data like so:
 *     char process_backing_store[cte_decode_process_size(max_depth)];
 *     struct cte_decode_process* decode_process = (struct cte_decode_process*)process_backing_store;
 * or
 *     struct cte_decode_process* decode_process = (struct cte_decode_process*)malloc(cte_decode_process_size());
 * or
 *     std::vector<char> process_backing_store(cte_decode_process_size(max_depth));
 *     struct cte_decode_process* decode_process = (struct cte_decode_process*)process_backing_store.data();
 *
 * @param max_container_depth The maximum container depth to suppport (<=0 means use default).
 * @return The process data size.
 */
CTE_PUBLIC int cte_decode_process_size(int max_container_depth);

/**
 * Begin a new decoding process.
 *
 * Successful status codes:
 * - CTE_DECODE_STATUS_OK: The decode process has begun.
 *
 * Unrecoverable codes:
 * - CTE_DECODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 *
 * @param decode_process The decode process to initialize.
 * @param callbacks The callbacks to call while decoding the document.
 * @param user_context Whatever data you want to be available to the callbacks.
 * @param max_container_depth The maximum container depth to suppport (<=0 means use default).
 * @return The current decoder status.
 */
CTE_PUBLIC cte_decode_status cte_decode_begin(struct cte_decode_process* decode_process,
                                              const cte_decode_callbacks* callbacks,
                                              void* user_context,
                                              int max_container_depth);

/**
 * Decode part of a CTE document.
 *
 * Note: data_start is not const because 
 *
 * Successful status codes:
 * - CTE_DECODE_STATUS_OK: document fragment was completely decoded.
 * - CTE_DECODE_STATUS_NEED_MORE_DATA: The document fragment was mostly decoded, and needs more data.
 *
 * Unrecoverable codes:
 * - CTE_DECODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_DECODE_ERROR_INVALID_ARRAY_DATA: An array type contained invalid data.
 * - CTE_DECODE_ERROR_UNBALANCED_CONTAINERS: a map or list is missing an end marker.
 * - CTE_DECODE_ERROR_INCORRECT_MAP_KEY_TYPE: a map has an invalid key type.
 * - CTE_DECODE_ERROR_MAP_MISSING_VALUE_FOR_KEY: document has a map key with no value.
 * - CTE_DECODE_ERROR_INCOMPLETE_ARRAY_FIELD: An array was not completed before document end.
 *
 * Recoverable codes:
 * - CTE_DECODE_STATUS_STOPPED_IN_CALLBACK: a callback function returned false.
 *
 * Encountering CTE_DECODE_STATUS_NEED_MORE_DATA means that cte_decode_feed()
 * has decoded everything it can from the current buffer, and is ready to
 * receive the next buffer of encoded data. THE PROCESS MAY NOT HAVE CONSUMED
 * ALL BYTES IN THE BUFFER. You must read the output value in byte_count to see
 * how many bytes were consumed, move the unconsumed bytes to the beginning of
 * the new buffer, add more bytes after that, then call cbe_decode_feed() again.
 *
 * If an unrecoverable code is returned, the document is invalid, and must be
 * discarded.
 *
 * @param decode_process The decode process.
 * @param data_start The start of the document.
 * @param byte_count In: The length of the data in bytes. Out: Number of bytes consumed.
 * @return The current decoder status.
 */
CTE_PUBLIC cte_decode_status cte_decode_feed(struct cte_decode_process* decode_process,
                                             const char* data_start,
                                             int64_t* byte_count);

/**
 * Get the current line offset into the overall stream of data.
 *
 * @param decode_process The decode process.
 * @return The current line offset.
 */
CTE_PUBLIC int64_t cte_decode_get_stream_line_offset(struct cte_decode_process* decode_process);

/**
 * Get the character offset into the current line in the overall stream of data.
 *
 * @param decode_process The decode process.
 * @return The current character offset.
 */
CTE_PUBLIC int64_t cte_decode_get_stream_character_offset(struct cte_decode_process* decode_process);

/**
 * End a decoding process, checking for document validity.
 *
 * Successful status codes:
 * - CTE_DECODE_STATUS_OK: document has been completely decoded.
 *
 * Unrecoverable codes:
 * - CTE_DECODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_DECODE_ERROR_UNBALANCED_CONTAINERS: one or more containers were not closed.
 * - CTE_DECODE_ERROR_INCOMPLETE_ARRAY_FIELD: an array field has not been completely filled yet.
 *
 * @param decode_process The decode process.
 * @return The final decoder status.
 */
CTE_PUBLIC cte_decode_status cte_decode_end(struct cte_decode_process* decode_process);


// ------------
// Encoding API
// ------------

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
     * One or more of the arguments was invalid.
     */
    CTE_ENCODE_ERROR_INVALID_ARGUMENT,

    /**
     * The array data was invalid.
     */
    CTE_ENCODE_ERROR_INVALID_ARRAY_DATA,

    /**
     * Unbalanced list/map begin and end markers were detected.
     */
    CTE_ENCODE_ERROR_UNBALANCED_CONTAINERS,

    /**
     * An invalid data type was used as a map key.
     */
    CTE_ENCODE_ERROR_INCORRECT_MAP_KEY_TYPE,

    /**
     * A map contained a key with no value.
     */
    CTE_ENCODE_ERROR_MAP_MISSING_VALUE_FOR_KEY,

    /**
     * Attempted to add a new array field or close the document before the
     * currently open field was completely filled.
     */
    CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD,

    /**
     * The currently open field is smaller than the data being added.
     */
    CTE_ENCODE_ERROR_ARRAY_FIELD_LENGTH_EXCEEDED,

    /**
     * We're not inside an array field.
     */
    CTE_ENCODE_ERROR_NOT_INSIDE_ARRAY_FIELD,

    /**
     * Max container depth (default 500) was exceeded.
     */
    CTE_ENCODE_ERROR_MAX_CONTAINER_DEPTH_EXCEEDED,

} cte_encode_status;

typedef enum
{
    CTE_BINARY_ENCODING_RADIX_16 = 16,
    CTE_BINARY_ENCODING_RADIX_85 = 85,
} cte_binary_encoding_radix;

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
 * @param max_container_depth The maximum container depth to suppport (<=0 means use default).
 * @return The process data size.
 */
CTE_PUBLIC int cte_encode_process_size(int max_container_depth);

/**
 * Begin a new encoding process.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The operation was successful.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 *
 * @param encode_process The encode process to initialize.
 * @param document_buffer A buffer to store the document in.
 * @param byte_count Size of the buffer in bytes.
 * @param max_container_depth The maximum container depth to suppport (<= 0 means use default).
 * @param indent_spaces The number of spaces to indent for pretty printing (0 = don't pretty print, < 0 use default).
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_begin(struct cte_encode_process* encode_process,
                                              uint8_t* const document_buffer,
                                              int64_t byte_count,
                                              int max_container_depth,
                                              int indent_spaces);

/**
 * Replace the document buffer in an encode process.
 * This also resets the buffer offset.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The operation was successful.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 *
 * @param encode_process The encode process.
 * @param document_buffer A buffer to store the document in.
 * @param byte_count Size of the buffer in bytes.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_set_buffer(struct cte_encode_process* encode_process,
                                                   uint8_t* document_buffer,
                                                   int64_t byte_count);

/**
 * Get the current write offset into the encode buffer.
 * This points to one past the last byte written to the current buffer.
 *
 * When a function returns CBE_ENCODE_STATUS_NEED_MORE_ROOM, you'll need to
 * flush the buffer up to this point, then call cbe_encode_set_buffer() again.
 *
 * @param encode_process The encode process.
 * @return The current offset.
 */
CTE_PUBLIC int64_t cte_encode_get_buffer_offset(struct cte_encode_process* encode_process);

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
CTE_PUBLIC int cte_encode_get_document_depth(struct cte_encode_process* encode_process);

/**
 * End an encoding process, checking the document for validity.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The encode process has ended.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_UNBALANCED_CONTAINERS: one or more containers were not closed.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: a field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @return The final encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_end(struct cte_encode_process* encode_process);

/**
 * Add a nil object to the document.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The operation was successful.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INCORRECT_MAP_KEY_TYPE: this can't be used as a map key.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_add_nil(struct cte_encode_process* encode_process);

/**
 * Add a boolean value to the document.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The operation was successful.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_add_boolean(struct cte_encode_process* encode_process, bool value);

/**
 * Add an integer value to the document.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The operation was successful.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_add_int(struct cte_encode_process* encode_process, int value);

/**
 * Add an integer value to the document.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The operation was successful.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_add_int_8(struct cte_encode_process* encode_process, int8_t value);

/**
 * Add a 16 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The operation was successful.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_add_int_16(struct cte_encode_process* encode_process, int16_t value);

/**
 * Add a 32 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The operation was successful.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_add_int_32(struct cte_encode_process* encode_process, int32_t value);

/**
 * Add a 64 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The operation was successful.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_add_int_64(struct cte_encode_process* encode_process, int64_t value);

/**
 * Add a 128 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The operation was successful.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_add_int_128(struct cte_encode_process* encode_process, int128_ct value);

/**
 * Add a 32 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The operation was successful.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param precision The maximum number of digits of precision to output after the decimal point.
 * @param value The value to add.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_add_float_32(struct cte_encode_process* encode_process,
                                                     int precision,
                                                     float value);

/**
 * Add a 64 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The operation was successful.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param precision The maximum number of digits of precision to output after the decimal point.
 * @param value The value to add.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_add_float_64(struct cte_encode_process* encode_process,
                                                     int precision,
                                                     double value);

/**
 * Add a 128 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The operation was successful.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param precision The maximum number of digits of precision to output after the decimal point.
 * @param value The value to add.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_add_float_128(struct cte_encode_process* encode_process,
                                                      int precision,
                                                      float128_ct value);

/**
 * Add a 32 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The operation was successful.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_add_decimal_32(struct cte_encode_process* encode_process, dec32_ct value);

/**
 * Add a 64 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The operation was successful.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_add_decimal_64(struct cte_encode_process* encode_process, dec64_ct value);

/**
 * Add a 128 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The operation was successful.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_add_decimal_128(struct cte_encode_process* encode_process, dec128_ct value);

/**
 * Add a time value to the document.
 * Use cte_new_time() to generate a time value.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The operation was successful.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_add_time(struct cte_encode_process* encode_process, smalltime value);

/**
 * Begin a list in the document. Must be matched by an end container.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The list has been opened.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INCORRECT_MAP_KEY_TYPE: this can't be used as a map key.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 * - CTE_ENCODE_ERROR_MAX_CONTAINER_DEPTH_EXCEEDED: container depth too deep.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_list_begin(struct cte_encode_process* encode_process);

/**
 * Begin a map in the document. Must be matched by an end container.
 *
 * Map entries must be added in pairs. Every even item is a key, and every
 * odd item is a corresponding value.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The map has been opened.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INCORRECT_MAP_KEY_TYPE: this can't be used as a map key.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 * - CTE_ENCODE_ERROR_MAX_CONTAINER_DEPTH_EXCEEDED: container depth too deep.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_map_begin(struct cte_encode_process* encode_process);

/**
 * End the current container (list or map) in the document.
 * If calling this function would result in too many end containers,
 * the operation is aborted and returns CTE_ENCODE_ERROR_UNBALANCED_CONTAINERS
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The container has been closed.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 * - CTE_ENCODE_ERROR_UNBALANCED_CONTAINERS: we're not in a container.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_container_end(struct cte_encode_process* encode_process);

/**
 * Convenience function: add a UTF-8 encoded string and its data to a document.
 * This function does not preserve partial data in the encoded buffer. Either the
 * entire operation succeeds, or it fails, restoring the buffer offset to where it
 * was before adding the array header.
 * Note: Do not include a byte order marker (BOM).
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The operation was successful.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INVALID_ARRAY_DATA: The string contained invalid data.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param string_start The string to add. May be NULL iff byte_count = 0.
 * @param byte_count The number of bytes in the string.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_add_string(struct cte_encode_process* encode_process,
                                                   const char* string_start,
                                                   int64_t byte_count);

/**
 * Convenience function: add a binary data blob to a document.
 * This function does not preserve partial data in the encoded buffer. Either the
 * entire operation succeeds, or it fails, restoring the buffer offset to where it
 * was before adding the array header.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The operation was successful.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param radix Which radix to encode at.
 * @param data The data to add. May be NULL iff byte_count = 0.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_add_binary(struct cte_encode_process* encode_process,
                                                   cte_binary_encoding_radix radix,
                                                   const uint8_t* data,
                                                   int64_t byte_count);

/**
 * Convenience function: add a UTF-8 encoded comment and its data to a document.
 * This function does not preserve partial data in the encoded buffer. Either the
 * entire operation succeeds, or it fails, restoring the buffer offset to where it
 * was before adding the array header.
 * Note: Do not include a byte order marker (BOM).
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The operation was successful.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INVALID_ARRAY_DATA: The comment contained invalid data.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param comment_start The comment to add. May be NULL iff byte_count = 0.
 * @param byte_count The number of bytes in the string.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_add_comment(struct cte_encode_process* encode_process,
                                                    const char* comment_start,
                                                    int64_t byte_count);

/**
 * Begin a string in the document. The string data will be UTF-8 without a BOM.
 *
 * This function "opens" a string field, encoding the type and length portions.
 * The encode process will expect subsequent cte_encode_add_data() calls
 * to fill up the field, and a final call to cte_encode_array_end() to close the field.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The string has been opened.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_string_begin(struct cte_encode_process* encode_process);

/**
 * Begin an array of binary data in the document.
 *
 * This function "opens" a binary field, encoding the type and length portions.
 * The encode process will expect subsequent cte_encode_add_binary_data() calls
 * to fill up the field, and a final call to cte_encode_array_end() to close the field.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The binary array has been opened.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param radix Which radix to encode at.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_binary_begin(struct cte_encode_process* encode_process,
                                                     cte_binary_encoding_radix radix);

/**
 * Begin a comment in the document. The comment data will be UTF-8 without a BOM.
 *
 * This function "opens" a comment field, encoding the type and length portions.
 * The encode process will expect subsequent cte_encode_add_data() calls
 * to fill up the field, and a final call to cte_encode_comment_end() to close the field.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The comment has been opened.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD: an open array field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_comment_begin(struct cte_encode_process* encode_process);

/**
 * Add data to the currently opened array field (string, binary, comment).
 *
 * If there's not enough room in the buffer, this function will add as much
 * data as it can to the encoded buffer before setting the output value of
 * byte_count to the number of bytes consumed, and then returning
 * CBE_ENCODE_STATUS_NEED_MORE_ROOM.
 *
 * Once you've finished adding the data for your array field, call
 * cte_encode_array_end() to close the field.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The operation was successful.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_INVALID_ARRAY_DATA: The array contained invalid data.
 * - CTE_ENCODE_ERROR_NOT_INSIDE_ARRAY_FIELD: we're not inside an array field.
 *
 * @param encode_process The encode process.
 * @param start The start of the data to add. May be NULL iff *byte_count = 0.
 * @param byte_count In: The length of the data in bytes. Out: Number of bytes consumed.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_add_data(struct cte_encode_process* encode_process,
                                                 const uint8_t* start,
                                                 int64_t* byte_count);

/**
 * End a string field.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The string has been closed.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_NOT_INSIDE_ARRAY_FIELD: we're not inside an array field.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_string_end(struct cte_encode_process* encode_process);

/**
 * End a binary array field.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The binary array has been closed.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_NOT_INSIDE_ARRAY_FIELD: we're not inside an array field.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_binary_end(struct cte_encode_process* encode_process);

/**
 * End a comment field.
 *
 * Successful status codes:
 * - CTE_ENCODE_STATUS_OK: The comment has been closed.
 *
 * Recoverable codes:
 * - CTE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CTE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CTE_ENCODE_ERROR_NOT_INSIDE_ARRAY_FIELD: we're not inside an array field.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
CTE_PUBLIC cte_encode_status cte_encode_comment_end(struct cte_encode_process* encode_process);


#ifdef __cplusplus
}
#endif
