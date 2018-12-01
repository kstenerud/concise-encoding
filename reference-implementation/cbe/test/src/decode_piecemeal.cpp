#include "test_helpers.h"

// Test working with partial buffers to simulate a streaming environment

TEST(DecodePiecemeal, piecemeal)
{
	std::vector<uint8_t> const& expected_memory = {0x91, 0x01, 0x68, 0x00, 0x10, 0x93};
	std::vector<uint8_t> actual_memory =          {0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f};
	cbe_encode_process* outer_encode_process = cbe_encode_begin(actual_memory.data(), actual_memory.size());
	cbe_decode_callbacks callbacks;
	cbe_decode_process* decode_process = new_decode_encode_process(&callbacks, outer_encode_process);
	int offset = 0;
	int length = 3;
	EXPECT_EQ(CBE_DECODE_STATUS_NEED_MORE_DATA, cbe_decode_feed(decode_process, expected_memory.data() + offset, length));
	offset = cbe_decode_get_buffer_offset(decode_process);
	length = actual_memory.size() - offset;
	EXPECT_EQ(CBE_DECODE_STATUS_OK, cbe_decode_feed(decode_process, expected_memory.data() + offset, length));
	EXPECT_EQ(CBE_DECODE_STATUS_OK, cbe_decode_end(decode_process));
	EXPECT_EQ(expected_memory, actual_memory);
}
