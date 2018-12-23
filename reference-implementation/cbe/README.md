Reference Implementation for Concise Binary Encoding
====================================================

A C implementation to demonstrate a simple CBE codec.


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


Usage
-----

### Decoding

    char decode_process_backing_store[cbe_decode_process_size()];
    struct cbe_decode_process* decode_process = (struct cbe_decode_process*)decode_process_backing_store;
    const cbe_decode_callbacks callbacks =
    {
        // TODO: Fill in callback pointers
    };
    void* context = my_get_context_data();
    unsigned char* decode_buffer;
    int64_t bytes_received;
    cbe_decode_status status = CBE_DECODE_STATUS_OK;

    cbe_decode_begin(decode_process, &callbacks, context);

    while(my_has_more_data())
    {
        my_get_next_packet(&decode_buffer, &bytes_received);
        status = cbe_decode_feed(decode_process, decode_buffer, bytes_received);
        if(status != CBE_DECODE_STATUS_OK)
        {
            // TODO: Do something about it
        }
        int64_t bytes_consumed = cbe_decode_get_buffer_offset(decode_process);
        // Todo: Move uncomsumed bytes to the beginning of the buffer for next time around
    }

    status = cbe_decode_end(decode_process);


### Encoding

    char encode_process_backing_store[cbe_encode_process_size()];
    struct cbe_encode_process* encode_process = (struct cbe_encode_process*)encode_process_backing_store;
    void* context = my_get_context_data();
    unsigned char* document_buffer = my_get_document_pointer();
    int64_t document_buffer_size = my_get_document_byte_count();
    cbe_encode_status status = CBE_ENCODE_STATUS_OK;

    cbe_encode_begin(encode_process, document_buffer, document_buffer_size);
    status = cbe_encode_begin_list(encode_process);
    if(status != CBE_ENCODE_STATUS_OK)
    {
        // TODO: Do something about it
    }
    status = cbe_encode_add_int_8(encode_process, 1);
    status = cbe_encode_add_string(encode_process, "Testing");
    status = cbe_encode_end_container(encode_process);
    status = cbe_encode_end(encode_process);



Running Tests
-------------

    ./test/cbe_test
