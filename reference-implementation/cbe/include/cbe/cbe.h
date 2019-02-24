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



// ========
// Defaults
// ========

#ifndef CBE_DEFAULT_MAX_CONTAINER_DEPTH
    #define CBE_DEFAULT_MAX_CONTAINER_DEPTH 500
#endif


// -----------
// Library API
// -----------

/**
 * Get the current library version as a semantic version (e.g. "1.5.2").
 *
 * @return The library version.
 */
const char* cbe_version();



// ------------
// Decoding API
// ------------

struct cbe_decode_process;

/**
 * Status codes that can be returned by decoder functions.
 */
typedef enum
{
    /**
     * The document has been successfully decoded.
     */
    CBE_DECODE_STATUS_OK = 0,

    /**
     * The decoder has reached the end of the buffer and needs more data to
     * finish decoding the document.
     */
    CBE_DECODE_STATUS_NEED_MORE_DATA = 100,

    /**
     * A user callback returned false, stopping the decode process.
     * The process may be resumed after fixing whatever problem caused the
     * callback to return false.
     */
    CBE_DECODE_STATUS_STOPPED_IN_CALLBACK,

    /**
     * One or more of the arguments was invalid.
     */
    CBE_DECODE_ERROR_INVALID_ARGUMENT,

    /**
     * The array data was invalid.
     */
    CBE_DECODE_ERROR_INVALID_DATA,

    /**
     * Unbalanced list/map begin and end markers were detected.
     */
    CBE_DECODE_ERROR_UNBALANCED_CONTAINERS,

    /**
     * An invalid data type was used as a map key.
     */
    CBE_DECODE_ERROR_INCORRECT_KEY_TYPE,

    /**
     * A map contained a key with no value.
     */
    CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY,

    /**
     * Attempted to add a new field before the existing field was completely
     * filled.
     */
    CBE_DECODE_ERROR_INCOMPLETE_FIELD,

    /**
     * The currently open field is smaller than the data being added.
     */
    CBE_DECODE_ERROR_FIELD_LENGTH_EXCEEDED,

    /**
     * An internal bug triggered an error.
     */
    CBE_DECODE_ERROR_INTERNAL,

} cbe_decode_status;

/**
 * Callbacks for use with cbe_decode(). These mirror the encode API.
 *
 * cbe_decode() will call these callbacks as it decodes objects in the document.
 *
 * Callback functions return true if processing should continue.
 */
typedef struct
{
    // A nil field was decoded.
    bool (*on_nil) (struct cbe_decode_process* decode_process);

    // An boolean field was decoded.
    bool (*on_boolean) (struct cbe_decode_process* decode_process, bool value);

    // An 8-bit integer field was decoded.
    bool (*on_int_8) (struct cbe_decode_process* decode_process, int8_t value);

    // A 16-bit integer field was decoded.
    bool (*on_int_16) (struct cbe_decode_process* decode_process, int16_t value);

    // A 32-bit integer field was decoded.
    bool (*on_int_32) (struct cbe_decode_process* decode_process, int32_t value);

    // A 64-bit integer field was decoded.
    bool (*on_int_64) (struct cbe_decode_process* decode_process, int64_t value);

    // A 128-bit integer field was decoded.
    bool (*on_int_128) (struct cbe_decode_process* decode_process, __int128 value);

    // A 32-bit binary floating point field was decoded.
    bool (*on_float_32) (struct cbe_decode_process* decode_process, float value);

    // A 64-bit binary floating point field was decoded.
    bool (*on_float_64) (struct cbe_decode_process* decode_process, double value);

    // A 128-bit binary floating point field was decoded.
    bool (*on_float_128) (struct cbe_decode_process* decode_process, __float128 value);

    // A 32-bit decimal floating point field was decoded.
    bool (*on_decimal_32) (struct cbe_decode_process* decode_process, _Decimal32 value);

    // A 64-bit decimal floating point field was decoded.
    bool (*on_decimal_64) (struct cbe_decode_process* decode_process, _Decimal64 value);

    // A 128-bit decimal floating point field was decoded.
    bool (*on_decimal_128) (struct cbe_decode_process* decode_process, _Decimal128 value);

    // A time field was decoded.
    bool (*on_time) (struct cbe_decode_process* decode_process, smalltime value);

    // A list has been opened.
    bool (*on_list_begin) (struct cbe_decode_process* decode_process);

    // The current list has been closed.
    bool (*on_list_end) (struct cbe_decode_process* decode_process);

    // A map has been opened.
    bool (*on_map_begin) (struct cbe_decode_process* decode_process);

    // The current map has been closed.
    bool (*on_map_end) (struct cbe_decode_process* decode_process);

    /**
     * A string has been opened. Expect subsequent calls to
     * on_string_data() until the array has been filled. Once `byte_count`
     * bytes have been added via `on_data`, the field is considered
     * "complete" and is closed.
     *
     * @param decode_process The decode process.
     * @param byte_count The total length of the string.
     */
    bool (*on_string_begin) (struct cbe_decode_process* decode_process, int64_t byte_count);

    /**
     * String data was decoded, and should be added to the current string field.
     *
     * @param decode_process The decode process.
     * @param start The start of the data.
     * @param byte_count The number of bytes in this array fragment.
     */
    bool (*on_string_data) (struct cbe_decode_process* decode_process,
                            const char* start,
                            int64_t byte_count);

    /**
     * A binary data array has been opened. Expect subsequent calls to
     * on_binary_data() until the array has been filled. Once `byte_count`
     * bytes have been added via `on_data`, the field is considered
     * "complete" and is closed.
     *
     * @param decode_process The decode process.
     * @param byte_count The total length of the array.
     */
    bool (*on_binary_begin) (struct cbe_decode_process* decode_process, int64_t byte_count);

    /**
     * Binary data was decoded, and should be added to the current binary field.
     *
     * @param decode_process The decode process.
     * @param start The start of the data.
     * @param byte_count The number of bytes in this array fragment.
     */
    bool (*on_binary_data) (struct cbe_decode_process* decode_process,
                            const uint8_t* start,
                            int64_t byte_count);

    /**
     * A comment has been opened. Expect subsequent calls to
     * on_comment_data() until the array has been filled. Once `byte_count`
     * bytes have been added via `on_data`, the field is considered
     * "complete" and is closed.
     *
     * @param decode_process The decode process.
     * @param byte_count The total length of the comment.
     */
    bool (*on_comment_begin) (struct cbe_decode_process* decode_process, int64_t byte_count);

    /**
     * String data was decoded, and should be added to the current comment field.
     *
     * @param decode_process The decode process.
     * @param start The start of the data.
     * @param byte_count The number of bytes in this array fragment.
     */
    bool (*on_comment_data) (struct cbe_decode_process* decode_process,
                             const char* start,
                             int64_t byte_count);
} cbe_decode_callbacks;


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
void* cbe_decode_get_user_context(struct cbe_decode_process* decode_process);

/**
 * Decode an entire CBE document.
 *
 * Successful status codes:
 * - CBE_DECODE_STATUS_OK: document has been completely decoded.
 *
 * Unrecoverable codes:
 * - CBE_DECODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_DECODE_ERROR_UNBALANCED_CONTAINERS: a map or list is missing an end marker.
 * - CBE_DECODE_ERROR_INCORRECT_KEY_TYPE: document has an invalid key type.
 * - CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY: document has a map key with no value.
 * - CBE_DECODE_ERROR_INCOMPLETE_FIELD: An array was not completed before document end.
 * - CBE_DECODE_STATUS_STOPPED_IN_CALLBACK: a callback function returned false.
 *
 * @param callbacks The callbacks to call while decoding the document.
 * @param user_context Whatever data you want to be available to the callbacks.
 * @param max_container_depth The maximum container depth to suppport (<=0 means use default).
 * @param document_start The start of the document.
 * @param byte_count The number of bytes in the document.
 */
cbe_decode_status cbe_decode(const cbe_decode_callbacks* callbacks,
                             void* user_context,
                             const uint8_t* document_start,
                             int64_t byte_count,
                             int max_container_depth);



// -----------------
// Decoder Power API
// -----------------

/**
 * Get the size of the decode process data.
 * Use this to create a backing store for the process data like so:
 *     char process_backing_store[cbe_decode_process_size(max_depth)];
 *     struct cbe_decode_process* decode_process = (struct cbe_decode_process*)process_backing_store;
 * or
 *     struct cbe_decode_process* decode_process = (struct cbe_decode_process*)malloc(cbe_decode_process_size());
 * or
 *     std::vector<char> process_backing_store(cbe_decode_process_size(max_depth));
 *     struct cbe_decode_process* decode_process = (struct cbe_decode_process*)process_backing_store.data();
 *
 * @param max_container_depth The maximum container depth to suppport (<=0 means use default).
 * @return The process data size.
 */
int cbe_decode_process_size(int max_container_depth);

/**
 * Begin a new decoding process.
 *
 * Successful status codes:
 * - CBE_DECODE_STATUS_OK: The decode process has begun.
 *
 * Unrecoverable codes:
 * - CBE_DECODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 *
 * @param decode_process The decode process to initialize.
 * @param callbacks The callbacks to call while decoding the document.
 * @param max_container_depth The maximum container depth to suppport (<=0 means use default).
 * @param user_context Whatever data you want to be available to the callbacks.
 * @return The current decoder status.
 */
cbe_decode_status cbe_decode_begin(struct cbe_decode_process* decode_process,
                                   const cbe_decode_callbacks* callbacks,
                                   void* user_context,
                                   int max_container_depth);

/**
 * Decode part of a CBE document.
 *
 * Note: data_start is not const because 
 *
 * Successful status codes:
 * - CBE_DECODE_STATUS_OK: document has been completely decoded.
 * - CBE_DECODE_STATUS_NEED_MORE_DATA: out of data but not at end of document.
 *
 * Unrecoverable codes:
 * - CBE_DECODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_DECODE_ERROR_UNBALANCED_CONTAINERS: a map or list is missing an end marker.
 * - CBE_DECODE_ERROR_INCORRECT_KEY_TYPE: document has an invalid key type.
 * - CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY: document has a map key with no value.
 * - CBE_DECODE_ERROR_INCOMPLETE_FIELD: An array was not completed before document end.
 *
 * Recoverable codes:
 * - CBE_DECODE_STATUS_STOPPED_IN_CALLBACK: a callback function returned false.
 *
 * Encountering CBE_DECODE_STATUS_NEED_MORE_DATA means that cbe_decode_feed()
 * has decoded everything it can from the current buffer, and is ready to
 * receive the next buffer of encoded data. It is important to get the
 * current buffer offset after receiving this status code, because some bytes
 * at the end of the current buffer may not have been consumed; these
 * unconsumed bytes must be prepended to the subsequent buffer for the next
 * call to cbe_decode_feed().
 *
 * If an unrecoverable code is returned, the document is invalid, and must be
 * discarded.
 *
 * If a recoverable code is returned, the decoder process points to the field
 * that caused the code to be returned. If you can fix the problem that led to
 * the recoverable code, you may run `cbe_decode_feed()` again using the same
 * buffer + the current offset from `cbe_decode_get_buffer_offset()`.
 *
 * @param decode_process The decode process.
 * @param data_start The start of the document.
 * @param byte_count The number of bytes in the document fragment.
 * @return The current decoder status.
 */
cbe_decode_status cbe_decode_feed(struct cbe_decode_process* decode_process,
                                  const uint8_t* data_start,
                                  int64_t byte_count);

/**
 * Get the current read offset into the decode buffer.
 * The offset points to one past the last byte consumed by cbe_decode_feed().
 *
 * @param decode_process The decode process.
 * @return The current offset.
 */
int64_t cbe_decode_get_buffer_offset(struct cbe_decode_process* decode_process);

/**
 * Get the current offset into the overall stream of data.
 * This is the total bytes read across all calls to cbe_decode_feed().
 *
 * @param decode_process The decode process.
 * @return The current offset.
 */
int64_t cbe_decode_get_stream_offset(struct cbe_decode_process* decode_process);

/**
 * End a decoding process, checking for document validity.
 *
 * Successful status codes:
 * - CBE_DECODE_STATUS_OK: document has been completely decoded.
 *
 * Unrecoverable codes:
 * - CBE_DECODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_DECODE_ERROR_UNBALANCED_CONTAINERS: one or more containers were not closed.
 * - CBE_DECODE_ERROR_INCOMPLETE_FIELD: a field has not been completely filled yet.
 *
 * @param decode_process The decode process.
 * @return The final decoder status.
 */
cbe_decode_status cbe_decode_end(struct cbe_decode_process* decode_process);


// ------------
// Encoding API
// ------------

struct cbe_encode_process;

/**
 * Status codes that can be returned by encoder functions.
 */
typedef enum
{
    /**
     * Completed successfully.
     */
    CBE_ENCODE_STATUS_OK = 0,

    /**
     * The encoder has reached the end of the buffer and needs more room to
     * encode this object.
     */
    CBE_ENCODE_STATUS_NEED_MORE_ROOM = 200,

    /**
     * One or more of the arguments was invalid.
     */
    CBE_ENCODE_ERROR_INVALID_ARGUMENT,

    /**
     * The array data was invalid.
     */
    CBE_ENCODE_ERROR_INVALID_DATA,

    /**
     * Unbalanced list/map begin and end markers were detected.
     */
    CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS,

    /**
     * An invalid data type was used as a map key.
     */
    CBE_ENCODE_ERROR_INCORRECT_KEY_TYPE,

    /**
     * A map contained a key with no value.
     */
    CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY,

    /**
     * Attempted to add a new field or close the document before the existing
     * field was completely filled.
     */
    CBE_ENCODE_ERROR_INCOMPLETE_FIELD,

    /**
     * The currently open field is smaller than the data being added.
     */
    CBE_ENCODE_ERROR_FIELD_LENGTH_EXCEEDED,

    /**
     * We're not inside an array field.
     */
    CBE_ENCODE_ERROR_NOT_INSIDE_ARRAY_FIELD,

    /**
     * Max container depth (default 500) was exceeded.
     */
    CBE_ENCODE_ERROR_MAX_CONTAINER_DEPTH_EXCEEDED,

} cbe_encode_status;


/**
 * Get the size of the encode process data.
 * Use this to create a backing store for the process data like so:
 *     char process_backing_store[cbe_encode_process_size()];
 *     struct cbe_encode_process* encode_process = (struct cbe_encode_process*)process_backing_store;
 * or
 *     struct cbe_encode_process* encode_process = (struct cbe_encode_process*)malloc(cbe_encode_process_size());
 * or
 *     std::vector<char> process_backing_store(cbe_encode_process_size());
 *     struct cbe_encode_process* encode_process = (struct cbe_encode_process*)process_backing_store.data();
 *
 * @param max_container_depth The maximum container depth to suppport (<=0 means use default).
 * @return The process data size.
 */
int cbe_encode_process_size(int max_container_depth);

/**
 * Begin a new encoding process.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 *
 * @param encode_process The encode process to initialize.
 * @param document_buffer A buffer to store the document in.
 * @param byte_count Size of the buffer in bytes.
 * @param max_container_depth The maximum container depth to suppport (<=0 means use default).
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_begin(struct cbe_encode_process* encode_process,
                                   uint8_t* document_buffer,
                                   int64_t byte_count,
                                   int max_container_depth);

/**
 * Replace the document buffer in an encode process.
 * This also resets the buffer offset.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 *
 * @param encode_process The encode process.
 * @param document_buffer A buffer to store the document in.
 * @param byte_count Size of the buffer in bytes.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_set_buffer(struct cbe_encode_process* encode_process,
                                        uint8_t* document_buffer,
                                        int64_t byte_count);

/**
 * Get the current write offset into the encode buffer.
 * This points to one past the last byte written to the current buffer.
 *
 * @param encode_process The encode process.
 * @return The current offset.
 */
int64_t cbe_encode_get_buffer_offset(struct cbe_encode_process* encode_process);

/**
 * Get the current write offset into the array being encoded.
 * Use this value when encoding an array in multiple steps.
 * This value is only valid while encoding an array.
 *
 * @param encode_process The encode process.
 * @return The current offset.
 */
int64_t cbe_encode_get_array_offset(struct cbe_encode_process* encode_process);

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
int cbe_encode_get_document_depth(struct cbe_encode_process* encode_process);

/**
 * End an encoding process, checking the document for validity.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS: one or more containers were not closed.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: a field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @return The final encoder status.
 */
cbe_encode_status cbe_encode_end(struct cbe_encode_process* encode_process);

/**
 * Add padding to the document.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param byte_count The number of bytes of padding to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_padding(struct cbe_encode_process* encode_process, int byte_count);

/**
 * Add an nil object to the document.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCORRECT_KEY_TYPE: this can't be used as a map key.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_nil(struct cbe_encode_process* encode_process);

/**
 * Add a boolean value to the document.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_boolean(struct cbe_encode_process* encode_process, bool value);

/**
 * Add an integer value to the document.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_int(struct cbe_encode_process* encode_process, int value);

/**
 * Add an integer value to the document.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_int_8(struct cbe_encode_process* encode_process, int8_t value);

/**
 * Add a 16 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_int_16(struct cbe_encode_process* encode_process, int16_t value);

/**
 * Add a 32 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_int_32(struct cbe_encode_process* encode_process, int32_t value);

/**
 * Add a 64 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_int_64(struct cbe_encode_process* encode_process, int64_t value);

/**
 * Add a 128 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_int_128(struct cbe_encode_process* encode_process, __int128 value);

/**
 * Add a 32 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_float_32(struct cbe_encode_process* encode_process, float value);

/**
 * Add a 64 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_float_64(struct cbe_encode_process* encode_process, double value);

/**
 * Add a 128 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_float_128(struct cbe_encode_process* encode_process, __float128 value);

/**
 * Add a 32 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_decimal_32(struct cbe_encode_process* encode_process, _Decimal32 value);

/**
 * Add a 64 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_decimal_64(struct cbe_encode_process* encode_process, _Decimal64 value);

/**
 * Add a 128 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_decimal_128(struct cbe_encode_process* encode_process, _Decimal128 value);

/**
 * Add a time value to the document.
 * Use cbe_new_time() to generate a time value.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_time(struct cbe_encode_process* encode_process, smalltime value);

/**
 * Begin a list in the document. Must be matched by an end container.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCORRECT_KEY_TYPE: this can't be used as a map key.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 * - CBE_ENCODE_ERROR_MAX_CONTAINER_DEPTH_EXCEEDED: container depth too deep.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_list_begin(struct cbe_encode_process* encode_process);

/**
 * Begin a map in the document. Must be matched by an end container.
 *
 * Map entries must be added in pairs. Every even item is a key, and every
 * odd item is a corresponding value.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCORRECT_KEY_TYPE: this can't be used as a map key.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 * - CBE_ENCODE_ERROR_MAX_CONTAINER_DEPTH_EXCEEDED: container depth too deep.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_map_begin(struct cbe_encode_process* encode_process);

/**
 * End the current container (list or map) in the document.
 * If calling this function would result in too many end containers,
 * the operation is aborted and returns CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 * - CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS: we're not in a container.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_container_end(struct cbe_encode_process* encode_process);

/**
 * Convenience function: add a UTF-8 encoded string and its data to a document.
 * Note: Do not include a byte order marker (BOM).
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param str The string to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_string(struct cbe_encode_process* encode_process, const char* str);

/**
 * Convenience function: add a binary data blob to a document.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param data The data to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_binary(struct cbe_encode_process* encode_process,
                                        const uint8_t* data,
                                        int64_t byte_count);

/**
 * Convenience function: add a UTF-8 encoded comment and its data to a document.
 * Note: Do not include a byte order marker (BOM).
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param str The comment to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_comment(struct cbe_encode_process* encode_process, const char* str);

/**
 * Begin a string in the document. The string data will be UTF-8 without a BOM.
 *
 * This function "opens" a string field, encoding the type and length portions.
 * The encode process will expect subsequent cbe_encode_add_data() calls
 * to fill up the field.
 * Once the field has been filled, it is considered "closed", and other fields
 * may now be added to the document.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param byte_count The total length of the string to add in bytes.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_string_begin(struct cbe_encode_process* encode_process, int64_t byte_count);

/**
 * Begin an array of binary data in the document.
 *
 * This function "opens" a binary field, encoding the type and length portions.
 * The encode process will expect subsequent cbe_encode_add_binary_data() calls
 * to fill up the field.
 * Once the field has been filled, it is considered "closed", and other fields
 * may now be added to the document.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param byte_count The total length of the data to add in bytes.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_binary_begin(struct cbe_encode_process* encode_process, int64_t byte_count);

/**
 * Begin a comment in the document. The comment data will be UTF-8 without a BOM.
 *
 * This function "opens" a comment field, encoding the type and length portions.
 * The encode process will expect subsequent cbe_encode_add_data() calls
 * to fill up the field.
 * Once the field has been filled, it is considered "closed", and other fields
 * may now be added to the document.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_INCOMPLETE_FIELD: an existing field has not been completed yet.
 *
 * @param encode_process The encode process.
 * @param byte_count The total length of the comment to add in bytes.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_comment_begin(struct cbe_encode_process* encode_process, int64_t byte_count);

/**
 * Add data to the currently opened array field (string, binary, comment).
 * You can call this as many times as you like until the array field has been
 * completely filled, at which point it is automatically considered "completed"
 * and is closed.
 *
 * Successful status codes:
 * - CBE_ENCODE_STATUS_OK: The encode process has begun.
 *
 * Recoverable codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * Unrecoverable codes:
 * - CBE_ENCODE_ERROR_INVALID_ARGUMENT: One of the arguments was null or invalid.
 * - CBE_ENCODE_ERROR_FIELD_LENGTH_EXCEEDED: would add too much data to the field.
 * - CBE_ENCODE_ERROR_NOT_INSIDE_ARRAY_FIELD: we're not inside an array field.
 *
 * @param encode_process The encode process.
 * @param start The start of the data to add.
 * @param byte_count The length of the data in bytes.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_data(struct cbe_encode_process* encode_process,
                                      const uint8_t* start,
                                      int64_t byte_count);



#ifdef __cplusplus 
}
#endif
