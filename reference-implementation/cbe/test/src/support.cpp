#include "test_helpers.h"

TEST(SupportTest, create_buffer)
{
    uint8_t memory[100];
    uint8_t* expected_start = &memory[0];
    uint8_t* expected_pos = &memory[0];
    uint8_t* expected_end = &memory[sizeof(memory)];

    cbe_encode_context context = create_encode_context(memory, sizeof(memory));
    fflush(stdout);

    EXPECT_EQ(expected_start, context.start);
    EXPECT_EQ(expected_pos, context.pos);
    EXPECT_EQ(expected_end, context.end);
}
