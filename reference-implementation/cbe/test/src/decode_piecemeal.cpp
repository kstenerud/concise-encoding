#include "old_test_helpers.h"

// Test working with partial buffers to simulate a streaming environment

// TODO: This is broken.
// TEST(DecodePiecemeal, piecemeal)
// {
//     std::vector<uint8_t> const& expected_memory = {0x7c, 0x01, 0x6e, 0x00, 0x10, 0x7d};
//     std::vector<uint8_t> actual_memory =          {0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f};
//     const uint8_t* document_start = expected_memory.data();

//     struct cbe_encode_process* outer_encode_process = cbe_encode_begin(actual_memory.data(), actual_memory.size());
//     cbe_decode_callbacks callbacks;
//     struct cbe_decode_process* decode_process = new_decode_encode_process(&callbacks, outer_encode_process);
//     int offset = 0;
//     int length = 3;
//     EXPECT_EQ(CBE_DECODE_STATUS_NEED_MORE_DATA, cbe_decode_feed(decode_process, document_start + offset, length));
//     offset = cbe_decode_get_buffer_offset(decode_process);
//     length = actual_memory.size() - offset;
//     EXPECT_EQ(CBE_DECODE_STATUS_OK, cbe_decode_feed(decode_process, document_start + offset, length));
//     EXPECT_EQ(CBE_DECODE_STATUS_OK, cbe_decode_end(decode_process));
//     EXPECT_EQ(expected_memory, actual_memory);
// }

TEST(DecodePiecemeal, binary_piecemeal)
{
    const int byte_length = 50;
    const int buffer_size = 30;
    std::vector<uint8_t> expected_memory = make_values_of_length<uint8_t>(byte_length);
    expected_memory.insert(expected_memory.begin(), {0x7b, 0x91, 0xc8});
    expected_memory.insert(expected_memory.end(), {0x7d});
    std::vector<uint8_t> actual_memory = make_values_of_length<uint8_t>(expected_memory.size());
    struct cbe_encode_process* encode_process = cbe_encode_begin(actual_memory.data(), actual_memory.size());
    cbe_decode_callbacks callbacks;
    struct cbe_decode_process* decode_process = new_decode_encode_process(&callbacks, encode_process);
    buffered_decode_feed(decode_process, expected_memory, buffer_size);
    EXPECT_EQ(CBE_DECODE_STATUS_OK, cbe_decode_end(decode_process));
    EXPECT_EQ(expected_memory, actual_memory);
}
