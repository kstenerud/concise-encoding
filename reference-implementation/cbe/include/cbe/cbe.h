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

typedef enum
{
	// enum value & 0xff == sizeof(type)
    CBE_TYPE_BOOLEAN_8    = 0x000 + sizeof(bool),
    CBE_TYPE_INT_8        = 0x100 + sizeof(int8_t),
    CBE_TYPE_INT_16       = 0x200 + sizeof(int16_t),
    CBE_TYPE_INT_32       = 0x300 + sizeof(int32_t),
    CBE_TYPE_INT_64       = 0x400 + sizeof(int64_t),
    CBE_TYPE_INT_128      = 0x500 + sizeof(__int128),
    CBE_TYPE_FLOAT_32     = 0x600 + sizeof(float),
    CBE_TYPE_FLOAT_64     = 0x700 + sizeof(double),
    CBE_TYPE_FLOAT_128    = 0x800 + sizeof(__float128),
    CBE_TYPE_DECIMAL_32   = 0x900 + sizeof(_Decimal32),
    CBE_TYPE_DECIMAL_64   = 0xa00 + sizeof(_Decimal64),
    CBE_TYPE_DECIMAL_128  = 0xb00 + sizeof(_Decimal128),
    CBE_TYPE_TIME_64      = 0xc00 + sizeof(smalltime),
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
 * Status codes that can be returned by decoder functions.
 */
typedef enum
{
	/**
	 * The document has been successfully decoded.
	 */
	CBE_DECODE_STATUS_OK,

    /**
     * Unbalanced container begin and end markers were detected.
     */
    CBE_DECODE_STATUS_UNBALANCED_CONTAINERS,

    /**
     * An invalid data type was used as a map key.
     */
    CBE_DECODE_STATUS_INCORRECT_KEY_TYPE,

    /**
     * A map contained a key with no value.
     */
    CBE_DECODE_STATUS_MISSING_VALUE_FOR_KEY,

    /**
     * A user callback returned false, stopping the decode process.
     * The process may be resumed after fixing whatever problem caused the
     * callback to return false.
     */
    CBE_DECODE_STATUS_STOPPED_IN_CALLBACK,

    /**
     * The decoder has reached the end of the buffer and needs
     * more data to finish decoding the document.
     */
    CBE_DECODE_STATUS_NEED_MORE_DATA,
} cbe_decode_status;

/**
 * Callbacks for use with cbe_decode().
 *
 * cbe_decode() will call these callbacks as it decodes objects in the document.
 *
 * Callback functions return true if processing should continue.
 */
typedef struct
{
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
	bool (*on_time)              (cbe_decode_process* decode_process, smalltime value);
	bool (*on_begin_list)        (cbe_decode_process* decode_process);
	bool (*on_begin_map)         (cbe_decode_process* decode_process);
    bool (*on_end_container)     (cbe_decode_process* decode_process);
	bool (*on_string)            (cbe_decode_process* decode_process, char* start, int64_t byte_count);
	bool (*on_binary_data)       (cbe_decode_process* decode_process, uint8_t* start, int64_t byte_count);
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
 * @param decode_process The decode process.
 * @return The user context.
 */
void* cbe_decode_get_user_context(cbe_decode_process* decode_process);

/**
 * Decode part of a CBE document.
 *
 * Possible status codes:
 * - CBE_DECODE_STATUS_OK: document has been completely decoded successfully.
 * - CBE_DECODE_STATUS_UNBALANCED_CONTAINERS: document has unbalanced containers.
 * - CBE_DECODE_STATUS_INCORRECT_KEY_TYPE: document has an invalid key type.
 * - CBE_DECODE_STATUS_MISSING_VALUE_FOR_KEY: document has a map key with no value.
 * - CBE_DECODE_STATUS_NEED_MORE_DATA: out of data but not at end of document.
 * - CBE_DECODE_STATUS_STOPPED_IN_CALLBACK: a callback function returned false.
 *
 * @param decode_process The decode process.
 * @param data_start The start of the document.
 * @param byte_count The number of bytes in the document fragment.
 * @return The current decoder status.
 */
cbe_decode_status cbe_decode_feed(cbe_decode_process* decode_process, const uint8_t* const data_start, const int64_t byte_count);

/**
 * Get the current write offset into the decode buffer.
 *
 * @param decode_process The decode process.
 * @return The current offset.
 */
int64_t cbe_decode_get_buffer_offset(cbe_decode_process* decode_process);

/**
 * End a decoding process, freeing up any decoder resources used.
 * Note: This does NOT free the user-supplied decode buffer.
 * Note: Resources will be freed and process terminated EVEN ON ERROR.
 *
 * Possible error codes:
 * - CBE_DECODE_STATUS_UNBALANCED_CONTAINERS: one or more containers were not closed.
 *
 * @param decode_process The decode process.
 * @return The final decoder status.
 */
cbe_decode_status cbe_decode_end(cbe_decode_process* decode_process);



// ------------
// Encoding API
// ------------

typedef struct {} cbe_encode_process;

/**
 * Status codes that can be returned by encoder functions.
 */
typedef enum
{
	/**
	 * Completed successfully.
	 */
	CBE_ENCODE_STATUS_OK,

    /**
     * Unbalanced container begin and end markers were detected.
     */
	CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS,

    /**
     * An invalid data type was used as a map key.
     */
	CBE_ENCODE_STATUS_INCORRECT_KEY_TYPE,

    /**
     * A map contained a key with no value.
     */
	CBE_ENCODE_STATUS_MISSING_VALUE_FOR_KEY,

	/**
	 * Max container depth (default 500) was exceeded.
	 */
	CBE_ENCODE_STATUS_MAX_CONTAINER_DEPTH_EXCEEDED,

	/**
	 * The encoder has reached the end of the buffer and needs
	 * more room to encode this object.
	 */
	CBE_ENCODE_STATUS_NEED_MORE_ROOM,

} cbe_encode_status;

/**
 * Begin a new encoding process.
 *
 * @param document_buffer A buffer to store the document in.
 * @param byte_count Size of the buffer in bytes.
 * @return The new encode process.
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
int cbe_encode_get_document_depth(cbe_encode_process* encode_process);

/**
 * End an encoding process, freeing up any encoder resources used.
 * Note: This does NOT free the user-supplied encode buffer.
 * Note: Resources will be freed and process terminated EVEN ON ERROR.
 *
 * Possible error codes:
 * - CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS: one or more containers were not closed.
 *
 * @param encode_process The encode process.
 * @return The final encoder status.
 */
cbe_encode_status cbe_encode_end(cbe_encode_process* encode_process);

/**
 * Add an empty object to the document.
 *
 * Possible error codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CBE_ENCODE_STATUS_INCORRECT_KEY_TYPE: this can't be used as a map key.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_empty(cbe_encode_process* const encode_process);

/**
 * Add a boolean value to the document.
 *
 * Possible error codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_boolean(cbe_encode_process* const encode_process, const bool value);

/**
 * Add an integer value to the document.
 *
 * Possible error codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_int(cbe_encode_process* const encode_process, const int value);

/**
 * Add an integer value to the document.
 *
 * Possible error codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_int_8(cbe_encode_process* const encode_process, const int8_t value);

/**
 * Add a 16 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_int_16(cbe_encode_process* const encode_process, const int16_t value);

/**
 * Add a 32 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_int_32(cbe_encode_process* const encode_process, const int32_t value);

/**
 * Add a 64 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_int_64(cbe_encode_process* const encode_process, const int64_t value);

/**
 * Add a 128 bit integer value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_int_128(cbe_encode_process* const encode_process, const __int128 value);

/**
 * Add a 32 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_float_32(cbe_encode_process* const encode_process, const float value);

/**
 * Add a 64 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_float_64(cbe_encode_process* const encode_process, const double value);

/**
 * Add a 128 bit floating point value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_float_128(cbe_encode_process* const encode_process, const __float128 value);

/**
 * Add a 32 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_decimal_32(cbe_encode_process* const encode_process, const _Decimal32 value);

/**
 * Add a 64 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_decimal_64(cbe_encode_process* const encode_process, const _Decimal64 value);

/**
 * Add a 128 bit decimal value to the document.
 * Note that this will add a narrower type if it will fit.
 *
 * Possible error codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_decimal_128(cbe_encode_process* const encode_process, const _Decimal128 value);

/**
 * Add a time value to the document.
 * Use cbe_new_time() to generate a time value.
 *
 * Possible error codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param value The value to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_time(cbe_encode_process* const encode_process, const smalltime value);

/**
 * Begin a list in the document. Must be matched by an end container.
 *
 * Possible error codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CBE_ENCODE_STATUS_INCORRECT_KEY_TYPE: this can't be used as a map key.
 * - CBE_ENCODE_STATUS_MAX_CONTAINER_DEPTH_EXCEEDED: container depth too deep.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_begin_list(cbe_encode_process* const encode_process);

/**
 * Begin a map in the document. Must be matched by an end container.
 *
 * Map entries must be added in pairs. Every even item is a key, and every
 * odd item is a corresponding value.
 *
 * Possible error codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CBE_ENCODE_STATUS_INCORRECT_KEY_TYPE: this can't be used as a map key.
 * - CBE_ENCODE_STATUS_MAX_CONTAINER_DEPTH_EXCEEDED: container depth too deep.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_begin_map(cbe_encode_process* const encode_process);

/**
 * End the current container (list or map) in the document.
 * If calling this function would result in too many end containers,
 * the operation is aborted and returns CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS
 *
 * Possible error codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 * - CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS: we're not in a container.
 *
 * @param encode_process The encode process.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_end_container(cbe_encode_process* const encode_process);

/**
 * Add a UTF-8 encoded null-terminated string value to the document.
 * Do not include a byte order marker (BOM)
 *
 * Possible error codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param str The null-terminated string to add.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_string(cbe_encode_process* const encode_process, const char* const str);

/**
 * Add a substring of a UTF-8 encoded string value to the document.
 * Do not include a byte order marker (BOM)
 *
 * Possible error codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param string_start The start of the substring to add.
 * @param byte_count The length of the substring in bytes.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_substring(cbe_encode_process* const encode_process, const char* const string_start, const int64_t byte_count);

/**
 * Add an array of binary data to the document.
 *
 * Possible error codes:
 * - CBE_ENCODE_STATUS_NEED_MORE_ROOM: not enough room left in the buffer.
 *
 * @param encode_process The encode process.
 * @param start The start of the data.
 * @param byte_count The length of the data in bytes.
 * @return The current encoder status.
 */
cbe_encode_status cbe_encode_add_binary_data(cbe_encode_process* const encode_process, const uint8_t* const start, const int64_t byte_count);



#ifdef __cplusplus 
}
#endif
