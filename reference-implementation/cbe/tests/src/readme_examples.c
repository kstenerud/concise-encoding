#include <cbe/cbe.h>

static bool my_on_nil(struct cbe_decode_process* process)
{
    (void)process;
    return true;
}

static bool my_on_boolean(struct cbe_decode_process* process, bool value)
{
    (void)process;
    (void)value;
    return true;
}

static bool my_on_int(struct cbe_decode_process* process, int value)
{
    (void)process;
    (void)value;
    return true;
}

static bool my_on_int_64(struct cbe_decode_process* process, int64_t value)
{
    (void)process;
    (void)value;
    return true;
}

static bool my_on_int_128(struct cbe_decode_process* process, int128_ct value)
{
    (void)process;
    (void)value;
    return true;
}

static bool my_on_uint(struct cbe_decode_process* process, unsigned value)
{
    (void)process;
    (void)value;
    return true;
}

static bool my_on_uint_64(struct cbe_decode_process* process, uint64_t value)
{
    (void)process;
    (void)value;
    return true;
}

static bool my_on_uint_128(struct cbe_decode_process* process, uint128_ct value)
{
    (void)process;
    (void)value;
    return true;
}

static bool my_on_float_32(struct cbe_decode_process* process, float value)
{
    (void)process;
    (void)value;
    return true;
}

static bool my_on_float_64(struct cbe_decode_process* process, double value)
{
    (void)process;
    (void)value;
    return true;
}

static bool my_on_float_128(struct cbe_decode_process* process, float128_ct value)
{
    (void)process;
    (void)value;
    return true;
}

static bool my_on_decimal_32(struct cbe_decode_process* process, dec32_ct value)
{
    (void)process;
    (void)value;
    return true;
}

static bool my_on_decimal_64(struct cbe_decode_process* process, dec64_ct value)
{
    (void)process;
    (void)value;
    return true;
}

static bool my_on_decimal_128(struct cbe_decode_process* process, dec128_ct value)
{
    (void)process;
    (void)value;
    return true;
}

static bool my_on_time(struct cbe_decode_process* process, smalltime value)
{
    (void)process;
    (void)value;
    return true;
}

static bool my_on_list_begin(struct cbe_decode_process* process)
{
    (void)process;
    return true;
}

static bool my_on_list_end(struct cbe_decode_process* process)
{
    (void)process;
    return true;
}

static bool my_on_map_begin(struct cbe_decode_process* process)
{
    (void)process;
    return true;
}

static bool my_on_map_end(struct cbe_decode_process* process)
{
    (void)process;
    return true;
}

static bool my_on_string_begin(struct cbe_decode_process* process, int64_t byte_count)
{
    (void)process;
    (void)byte_count;
    return true;
}

static bool my_on_binary_begin(struct cbe_decode_process* process, int64_t byte_count)
{
    (void)process;
    (void)byte_count;
    return true;
}

static bool my_on_comment_begin(struct cbe_decode_process* process, int64_t byte_count)
{
    (void)process;
    (void)byte_count;
    return true;
}

static bool my_on_string_data(struct cbe_decode_process* process,
                           const char* start,
                           int64_t byte_count)
{
    (void)process;
    (void)start;
    (void)byte_count;
    return true;
}

static bool my_on_binary_data(struct cbe_decode_process* process,
                           const uint8_t* start,
                           int64_t byte_count)
{
    (void)process;
    (void)start;
    (void)byte_count;
    return true;
}

static bool my_on_comment_data(struct cbe_decode_process* process,
                           const char* start,
                           int64_t byte_count)
{
    (void)process;
    (void)start;
    (void)byte_count;
    return true;
}

static void my_report_decode_error(struct cbe_decode_process* process, cbe_decode_status status)
{
    (void)process;
    (void)status;
}

static void my_report_encode_error(struct cbe_encode_process* process, cbe_encode_status status)
{
    (void)process;
    (void)status;
}

static void my_flush_buffer(const uint8_t* buffer, int64_t buffer_length)
{
    (void)buffer;
    (void)buffer_length;
}

// ==================================================================

bool decode(const uint8_t* my_document, int64_t my_document_size, void* my_context_data)
{
    const int max_container_depth = 500;
    char backing_store[cbe_decode_process_size(max_container_depth)];
    struct cbe_decode_process* decode_process = (struct cbe_decode_process*)backing_store;
    const cbe_decode_callbacks callbacks =
    {
        .on_nil           = my_on_nil,
        .on_boolean       = my_on_boolean,
        .on_int           = my_on_int,
        .on_int_64        = my_on_int_64,
        .on_int_128       = my_on_int_128,
        .on_uint          = my_on_uint,
        .on_uint_64       = my_on_uint_64,
        .on_uint_128      = my_on_uint_128,
        .on_float_32      = my_on_float_32,
        .on_float_64      = my_on_float_64,
        .on_float_128     = my_on_float_128,
        .on_decimal_32    = my_on_decimal_32,
        .on_decimal_64    = my_on_decimal_64,
        .on_decimal_128   = my_on_decimal_128,
        .on_time          = my_on_time,
        .on_list_begin    = my_on_list_begin,
        .on_list_end      = my_on_list_end,
        .on_map_begin     = my_on_map_begin,
        .on_map_end       = my_on_map_end,
        .on_string_begin  = my_on_string_begin,
        .on_string_data   = my_on_string_data,
        .on_binary_begin  = my_on_binary_begin,
        .on_binary_data   = my_on_binary_data,
        .on_comment_begin = my_on_comment_begin,
        .on_comment_data  = my_on_comment_data,
    };

    cbe_decode_status status = cbe_decode(&callbacks,
                                          my_context_data,
                                          my_document,
                                          my_document_size,
                                          max_container_depth);
    if(status != CBE_DECODE_STATUS_OK)
    {
        my_report_decode_error(decode_process, status);
        return false;
    }
    return true;
}

bool encode()
{
    uint8_t buffer[1000];
    const int max_container_depth = 500;
    char backing_store[cbe_encode_process_size(max_container_depth)];
    struct cbe_encode_process* encode_process = (struct cbe_encode_process*)backing_store;

    cbe_encode_status status = cbe_encode_begin(encode_process,
                                                buffer,
                                                sizeof(buffer),
                                                max_container_depth);
    if(status != CBE_ENCODE_STATUS_OK)
    {
        my_report_encode_error(encode_process, status);
        return false;
    }

    // TODO: Check for errors on all cbe calls
    status = cbe_encode_list_begin(encode_process);
    if(status == CBE_ENCODE_STATUS_NEED_MORE_ROOM)
    {
        my_flush_buffer(buffer, cbe_encode_get_buffer_offset(encode_process));
        cbe_encode_set_buffer(encode_process, buffer, sizeof(buffer));
    }

    // TODO: Check for out of room on all cbe calls
    status = cbe_encode_add_int(encode_process, 1);
    status = cbe_encode_add_string(encode_process, "Testing", 7);
    status = cbe_encode_container_end(encode_process);
    status = cbe_encode_end(encode_process);

    if(status != CBE_ENCODE_STATUS_OK)
    {
        my_report_encode_error(encode_process, status);
        return false;
    }
    return true;
}
