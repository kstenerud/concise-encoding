#include "test_helpers.h"

TEST(SupportTest, create_buffer)
{
    uint8_t memory[100];
    int expected_offset = 0;

    cbe_encode_process* encode_process = cbe_encode_begin(memory, sizeof(memory));
    fflush(stdout);
    EXPECT_EQ(expected_offset, cbe_encode_get_buffer_offset(encode_process));
    EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_end(encode_process));
}
