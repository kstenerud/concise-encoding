Reference Implementation for Concise Binary Encoding
====================================================

A C implementation to demonstrate a CBE codec.


Assumptions
-----------

 * Assumes densely packed decimal encoding for C decimal types (_Decimal32, _Decimal64, _Decimal128). This is the default for gcc and other compilers using decNumber.
 * Assumes a little endian host.



Requirements
------------

  * Meson 0.49 or newer
  * Ninja 1.8.2 or newer
  * A C compiler
  * A C++ compiler (for the unit tests)



Dependencies
------------

 * decimal/decimal (if using C++): For C++ decimal float types
 * stdbool.h: For bool type
 * stdint.h: Fot int types



Building
--------

    meson build
    ninja -C build



Running Tests
-------------

    ninja -C build test

For the full report:

    ./build/run_tests



Installing
----------

    ninja -C build install



Usage
-----

### Decoding

```c
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
        .on_smalltime     = my_on_smalltime,
        .on_nanotime      = my_on_nanotime,
        .on_list_begin    = my_on_list_begin,
        .on_list_end      = my_on_list_end,
        .on_map_begin     = my_on_map_begin,
        .on_map_end       = my_on_map_end,
        .on_string_begin  = my_on_string_begin,
        .on_string_data   = my_on_string_data,
        .on_bytes_begin   = my_on_bytes_begin,
        .on_bytes_data    = my_on_bytes_data,
        .on_uri_begin     = my_on_uri_begin,
        .on_uri_data      = my_on_uri_data,
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
```


### Encoding

```c
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
```
