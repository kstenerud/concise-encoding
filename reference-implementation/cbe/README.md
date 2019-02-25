Reference Implementation for Concise Binary Encoding
====================================================

A C implementation to demonstrate a CBE codec.


Assumptions
-----------

 * Assumes densely packed decimal encoding for C decimal types (_Decimal32, _Decimal64, _Decimal128). This is the default for gcc and other compilers using decNumber.
 * Assumes a little endian host.



Requirements
------------

  * CMake 3.5 or higher
  * A C compiler
  * A C++ compiler (for the unit tests)



Dependencies
------------

 * decimal/decimal (if using C++): For C++ decimal float types
 * stdbool.h: For bool type
 * stdint.h: Fot int types



Building
--------

    mkdir build
    cd build
    cmake ..
    make



Running Tests
-------------

    ./test/cbe_test



Usage
-----

### Decoding

```c
    const int max_container_depth = 500;
    char backing_store[cbe_decode_process_size(max_container_depth)];
    struct cbe_decode_process* decode_process = (struct cbe_decode_process*)backing_store;
    const cbe_decode_callbacks callbacks =
    {
        .on_nil           = my_on_nil,
        .on_boolean       = my_on_boolean,
        .on_int_8         = my_on_int_8,
        .on_int_16        = my_on_int_16,
        .on_int_32        = my_on_int_32,
        .on_int_64        = my_on_int_64,
        .on_int_128       = my_on_int_128,
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
```


### Encoding

```c
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
    status = cbe_encode_add_int_8(encode_process, 1);
    status = cbe_encode_add_string(encode_process, "Testing", 7);
    status = cbe_encode_container_end(encode_process);
    status = cbe_encode_end(encode_process);

    if(status != CBE_ENCODE_STATUS_OK)
    {
        my_report_encode_error(encode_process, status);
        return false;
    }
    return true;
```
