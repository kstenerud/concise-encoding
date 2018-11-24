#include "test_helpers.h"

TEST(SupportTest, create_buffer)
{
    uint8_t memory[100];
    int expected_offset = 0;

    cbe_encode_context* context = cbe_encode_begin(memory, memory+sizeof(memory));
    fflush(stdout);
    EXPECT_EQ(expected_offset, cbe_encode_get_buffer_offset(context));
    cbe_encode_end(context);
}
