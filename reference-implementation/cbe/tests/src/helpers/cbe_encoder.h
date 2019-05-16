#pragma once

#include <functional>
#include <cbe/cbe.h>
#include "encoding.h"

class cbe_encoder: public enc::encoder
{
private:
    std::vector<char> _process_backing_store;
    cbe_encode_process* _process;
    std::vector<uint8_t> _buffer;
    std::vector<uint8_t> _encoded_data;
    int _max_container_depth;
    std::function<bool(uint8_t* data_start, int64_t length)> _on_data_ready;

protected:
    bool flush_buffer();
    cbe_encode_status flush_and_retry(std::function<cbe_encode_status()> my_function);

public:
    // Internal functions
    cbe_encode_status encode(const enc::list_encoding& encoding);
    cbe_encode_status encode(const enc::map_encoding& encoding);
    cbe_encode_status encode(const enc::container_end_encoding& encoding);
    cbe_encode_status encode(const enc::nil_encoding& encoding);
    cbe_encode_status encode(const enc::padding_encoding& encoding);
    cbe_encode_status encode(const enc::time_encoding& encoding);
    cbe_encode_status encode(const enc::string_encoding& encoding);
    cbe_encode_status encode(const enc::bytes_encoding& encoding);
    cbe_encode_status encode(const enc::comment_encoding& encoding);
    cbe_encode_status encode(const enc::string_header_encoding& encoding);
    cbe_encode_status encode(const enc::bytes_header_encoding& encoding);
    cbe_encode_status encode(const enc::comment_header_encoding& encoding);
    cbe_encode_status encode(const enc::data_encoding& encoding);
    cbe_encode_status encode(const enc::boolean_encoding& encoding);
    cbe_encode_status encode(const enc::int_encoding& encoding);
    cbe_encode_status encode(const enc::uint_encoding& encoding);
    cbe_encode_status encode(const enc::float_encoding& encoding);
    cbe_encode_status encode(const enc::decimal_encoding& encoding);
    cbe_encode_status stream_array(const std::vector<uint8_t>& data);

public:
    cbe_encoder(int64_t buffer_size,
                int max_container_depth,
                std::function<bool(uint8_t* data_start, int64_t length)> on_data_ready =
                    [](uint8_t* data_start, int64_t length)
                    {(void)data_start; (void)length; return true;});

    virtual ~cbe_encoder() {}

    // Encode an encoding object and all linked objects.
    cbe_encode_status encode(std::shared_ptr<enc::encoding> enc);

    // Encode entire array objects. If the entire object won't fit,
    // returns with failure.
    cbe_encode_status encode_string(std::vector<uint8_t> value);
    cbe_encode_status encode_bytes(std::vector<uint8_t> value);
    cbe_encode_status encode_comment(std::vector<uint8_t> value);

    int64_t get_encode_buffer_offset() const;

    // Get the complete raw encoded data.
    std::vector<uint8_t>& encoded_data() {return _encoded_data;}
};
