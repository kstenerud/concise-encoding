#include "test_helpers.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"


namespace cte_test
{

static std::string encode_data(int buffer_size,
                                        int max_container_depth,
                                        int indent_spaces,
                                        std::shared_ptr<enc::encoding> encoding,
                                        cte_encode_status& status)
{
    KSLOG_DEBUG("Encode %s with buffer size %d", encoding->as_string().c_str(), buffer_size);
    std::string actual_memory;
    cte_encoder encoder(buffer_size,
        max_container_depth,
        indent_spaces,
        [&](uint8_t* data_start, int64_t length)
        {
            KSLOG_DEBUG("Encoded data of length %d", length);
            actual_memory.insert(actual_memory.end(), data_start, data_start + length);
            return true;
        });
    status = encoder.encode(encoding);
    KSLOG_DEBUG("Status = %d", status);
    return actual_memory;
}

static std::shared_ptr<enc::encoding> decode_data(int buffer_size,
                                                  int max_container_depth,
                                                  bool callback_return_value,
                                                  std::string data,
                                                  cte_decode_status& status)
{
    KSLOG_DEBUG("Decode %d bytes with buffer size %d", data.size(), buffer_size);
    cte_decoder decoder(max_container_depth, callback_return_value);
    status = decoder.begin();
    if(status != CTE_DECODE_STATUS_OK)
    {
        KSLOG_DEBUG("decoder.begin() failed");
        return decoder.decoded();
    }

    for(unsigned offset = 0; offset < data.size(); offset += buffer_size)
    {
        std::string::iterator begin = data.begin() + offset;
        std::string::iterator end = data.end();
        if(offset + buffer_size < data.size())
        {
            end = data.begin() + offset + buffer_size;
        }
        KSLOG_DEBUG("Decoding data from %d for %d bytes", offset, buffer_size);
        std::vector<uint8_t> buffer(begin, end);
        status = decoder.feed(buffer);
        if(status != CTE_DECODE_STATUS_OK && status != CTE_DECODE_STATUS_NEED_MORE_DATA)
        {
            break;
        }
    }
    // Also checking NEED_MORE_DATA to allow testing for premature end of data.
    if(status == CTE_DECODE_STATUS_OK || status == CTE_DECODE_STATUS_NEED_MORE_DATA)
    {
        status = decoder.end();
    }
    KSLOG_DEBUG("Status = %d", status);
    return decoder.decoded();
}

static std::shared_ptr<enc::encoding> decode_oneshot(int max_container_depth,
                                                     bool callback_return_value,
                                                     std::string data,
                                                     cte_decode_status& status)
{
    KSLOG_DEBUG("Decode %d bytes", data.size());
    cte_decoder decoder(max_container_depth, callback_return_value);
    std::vector<uint8_t> binary_data(data.begin(), data.end());
    status = decoder.decode(binary_data);
    return decoder.decoded();
}

void expect_encode_produces_status(int buffer_size,
                                   int max_container_depth,
                                   int indent_spaces,
                                   std::shared_ptr<enc::encoding> encoding,
                                   cte_encode_status expected_encode_status)
{
    cte_encode_status encode_status = CTE_ENCODE_STATUS_OK;
    std::string memory = encode_data(buffer_size,
                                              max_container_depth,
                                              indent_spaces,
                                              encoding,
                                              encode_status);
    ASSERT_EQ(encode_status, expected_encode_status);
}

cte_encode_status expect_encode_produces_data_and_status(int buffer_size,
                                                         int max_container_depth,
                                                         int indent_spaces,
                                                         std::shared_ptr<enc::encoding> encoding,
                                                         const std::string expected_memory,
                                                         cte_encode_status expected_status)
{
    KSLOG_DEBUG("Encode with buffer size %d: %s", buffer_size, encoding->as_string().c_str());
    cte_encode_status actual_status = CTE_ENCODE_STATUS_OK;
    std::string actual_memory = encode_data(buffer_size,
                                                     max_container_depth,
                                                     indent_spaces,
                                                     encoding,
                                                     actual_status);
    EXPECT_EQ(expected_status, actual_status);
    if(actual_status == CTE_ENCODE_STATUS_OK)
    {
        EXPECT_EQ(expected_memory, actual_memory);
    }
    return actual_status;
}

void expect_decode_produces_status(int buffer_size,
                                   int max_container_depth,
                                   bool callback_return_value,
                                   std::string document,
                                   cte_decode_status expected_decode_status)
{
    cte_decode_status decode_status = CTE_DECODE_STATUS_OK;
    std::shared_ptr<enc::encoding> actual_encoding = decode_data(buffer_size,
                                                                 max_container_depth,
                                                                 callback_return_value,
                                                                 document,
                                                                 decode_status);
    ASSERT_EQ(decode_status, expected_decode_status);
}

void expect_decode_produces_data_and_status(int buffer_size,
                                            int max_container_depth,
                                            bool callback_return_value,
                                            const std::string memory,
                                            std::shared_ptr<enc::encoding> expected_encoding,
                                            cte_decode_status expected_status)
{
    KSLOG_DEBUG("Decode %s", as_string(memory).c_str());
    cte_decode_status actual_status = CTE_DECODE_STATUS_OK;
    std::shared_ptr<enc::encoding> actual_encoding = decode_data(buffer_size,
                                                                 max_container_depth,
                                                                 callback_return_value,
                                                                 memory,
                                                                 actual_status);
    ASSERT_EQ(expected_status, actual_status);
    ASSERT_TRUE(actual_encoding);
    ASSERT_EQ(*expected_encoding, *actual_encoding);
}

void expect_encode_decode_produces_status(int buffer_size,
                                          int max_container_depth,
                                          int indent_spaces,
                                          bool callback_return_value,
                                          std::shared_ptr<enc::encoding> encoding,
                                          cte_encode_status expected_encode_status,
                                          cte_decode_status expected_decode_status)
{
    cte_encode_status encode_status = CTE_ENCODE_STATUS_OK;
    cte_decode_status decode_status = CTE_DECODE_STATUS_OK;
    std::string memory = encode_data(buffer_size,
                                              max_container_depth,
                                              indent_spaces,
                                              encoding,
                                              encode_status);
    ASSERT_EQ(encode_status, expected_encode_status);
    std::shared_ptr<enc::encoding> actual_encoding = decode_oneshot(max_container_depth,
                                                                    callback_return_value,
                                                                    memory,
                                                                    decode_status);
    ASSERT_EQ(decode_status, expected_decode_status);
}

void expect_encode_decode_produces_data_and_status(int buffer_size,
                                                   int max_container_depth,
                                                   int indent_spaces,
                                                   std::shared_ptr<enc::encoding> expected_encoding,
                                                   const std::string expected_memory,
                                                   cte_encode_status expected_encode_status,
                                                   cte_decode_status expected_decode_status)
{
    if(expect_encode_produces_data_and_status(buffer_size,
                                              max_container_depth,
                                              indent_spaces,
                                              expected_encoding,
                                              expected_memory,
                                              expected_encode_status) == CTE_ENCODE_STATUS_OK)
    {
        const bool callback_return_value = true;
        expect_decode_produces_data_and_status(buffer_size,
                                               max_container_depth,
                                               callback_return_value,
                                               expected_memory,
                                               expected_encoding,
                                               expected_decode_status);
    }
}

void expect_encode_decode_with_shrinking_buffer_size(int min_buffer_size,
                                                     std::shared_ptr<enc::encoding> expected_encoding,
                                                     const std::string expected_memory)
{
    const int max_container_depth = 500;
    const int indent_spaces = 0;
    const int expected_buffer_size = expected_memory.size();
    if(min_buffer_size == 0)
    {
        min_buffer_size = expected_buffer_size;
    }
    for(int buffer_size = expected_buffer_size+16; buffer_size >= min_buffer_size; buffer_size--)
    {
        expect_encode_decode_produces_data_and_status(buffer_size,
                                                      max_container_depth,
                                                      indent_spaces,
                                                      expected_encoding,
                                                      expected_memory,
                                                      CTE_ENCODE_STATUS_OK,
                                                      CTE_DECODE_STATUS_OK);
    }
    if(min_buffer_size > 1)
    {
        expect_encode_produces_data_and_status(min_buffer_size - 1,
                                               max_container_depth,
                                               indent_spaces,
                                               expected_encoding,
                                               expected_memory,
                                               CTE_ENCODE_STATUS_NEED_MORE_ROOM);
    }
}

} // namespace cte_test
