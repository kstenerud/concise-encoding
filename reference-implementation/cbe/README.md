Reference Library for the Safe16 Encoding System
================================================

A C implementation to demonstrate a simple SAFE16 codec.


Requirements
------------

  * CMake 3.5 or higher
  * A C compiler
  * A C++ compiler (for the unit tests)


Dependencies
------------

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

    make test

or:

    ./tests/Safe16


Usage
-----

Note: Using C++ to make the string & data code simpler.

### Decoding

```c++
    std::string my_source_data = "21d17d3f21c18899714596adcc9679d8";

    int64_t decoded_length = safe16_get_decoded_length(my_source_data.size());
    std::vector<unsigned char> decode_buffer(decoded_length);

    int64_t used_bytes = safe16_decode(my_source_data.data(),
                                       my_source_data.size(),
                                       decode_buffer.data(),
                                       decode_buffer.size());
    if(used_bytes < 0)
    {
        // TODO: used_bytes is an error code.
    }
    std::vector<unsigned char> decoded_data(decode_buffer.begin(), decode_buffer.begin() + used_bytes);
    my_receive_decoded_data_function(decoded_data);
```

### Decoding (with length field)

```c++
    std::string my_source_data = "a021d17d3f21c18899714596adcc9679d8";

    int64_t decoded_length = safe16_get_decoded_length(my_source_data.size());
    std::vector<unsigned char> decode_buffer(decoded_length);

    int64_t used_bytes = safe16l_decode(my_source_data.data(),
                                        my_source_data.size(),
                                        decode_buffer.data(),
                                        decode_buffer.size());
    if(used_bytes < 0)
    {
        // TODO: used_bytes is an error code.
    }
    std::vector<unsigned char> decoded_data(decode_buffer.begin(), decode_buffer.begin() + used_bytes);
    my_receive_decoded_data_function(decoded_data);
```

### Encoding

```C++
    std::vector<unsigned char> my_source_data({0x39, 0x12, 0x82, 0xe1, 0x81, 0x39, 0xd9, 0x8b, 0x39, 0x4c, 0x63, 0x9d, 0x04, 0x8c});

    bool should_include_length = false;
    int64_t encoded_length = safe16_get_encoded_length(my_source_data.size(), should_include_length);
    std::vector<char> encode_buffer(encoded_length);

    int64_t used_bytes = safe16_encode(my_source_data.data(),
                                       my_source_data.size(),
                                       encode_buffer.data(),
                                       encode_buffer.size());
    if(used_bytes < 0)
    {
        // TODO: used_bytes is an error code.
    }
    std::string encoded_data(encode_buffer.begin(), encode_buffer.begin() + used_bytes);
    my_receive_encoded_data_function(encoded_data);
```

### Encoding (with length field)

```c++
    std::vector<unsigned char> my_source_data({0x39, 0x12, 0x82, 0xe1, 0x81, 0x39, 0xd9, 0x8b, 0x39, 0x4c, 0x63, 0x9d, 0x04, 0x8c});

    bool should_include_length = true;
    int64_t encoded_length = safe16_get_encoded_length(my_source_data.size(), should_include_length);
    std::vector<char> encode_buffer(encoded_length);

    int64_t used_bytes = safe16l_encode(my_source_data.data(),
                                        my_source_data.size(),
                                        encode_buffer.data(),
                                        encode_buffer.size());
    if(used_bytes < 0)
    {
        // TODO: used_bytes is an error code.
    }
    std::string encoded_data(encode_buffer.begin(), encode_buffer.begin() + used_bytes);
    my_receive_encoded_data_function(encoded_data);
```
