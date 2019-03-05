#pragma once

#include <functional>
#include <vector>
#include <cte/cte.h>
#include "encoding.h"

typedef enum
{
    CTE_DECODING_OTHER,
    CTE_DECODING_STRING,
    CTE_DECODING_BINARY,
    CTE_DECODING_COMMENT,
} cte_decoding_type;

class cte_decoder
{
private:
    std::vector<char> _process_backing_store;
    cte_decode_process* _process;
    std::vector<uint8_t> _received_data;
    bool _process_is_valid = true;
    std::shared_ptr<enc::encoding> _decoded;
    cte_decoding_type _currently_decoding_type = CTE_DECODING_OTHER;
    std::vector<uint8_t> _currently_decoding_data;
    bool _callback_return_value;
    int _max_container_depth;

public:
    // Internal functions
    bool set_next(std::shared_ptr<enc::encoding> encoding);
    bool string_begin();
    bool binary_begin();
    bool comment_begin();
    bool add_string_data(const std::string& data);
    bool add_binary_data(const std::vector<uint8_t>& data);
    bool add_comment_data(const std::string& data);
    bool string_end();
    bool binary_end();
    bool comment_end();

public:
    cte_decoder(int max_container_depth, bool callback_return_value);

    // Begin the decoding process.
    cte_decode_status begin();

    // Feed data to be decoded.
    cte_decode_status feed(std::vector<uint8_t>& data);

    // End the decoding process.
    cte_decode_status end();

    // Decode an entire document
    cte_decode_status decode(std::vector<uint8_t>& document);

    // Get the complete raw data received.
    std::vector<uint8_t>& received_data();

    // Get the decoded encoding objects.
    std::shared_ptr<enc::encoding> decoded();

    bool get_callback_return_value();
};
