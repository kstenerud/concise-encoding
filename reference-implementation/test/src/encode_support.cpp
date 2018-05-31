#include <stdio.h>
#include <gtest/gtest.h>
#include <cbe/encoder.h>
#include "encode_test_helpers.h"

TEST(SupportTest, create_buffer)
{
    uint8_t memory[100];
    uint8_t* expected_start = &memory[0];
    uint8_t* expected_pos = &memory[0];
    uint8_t* expected_end = &memory[sizeof(memory)];

    cbe_buffer buffer = create_buffer(memory, sizeof(memory));
    fflush(stdout);

    EXPECT_EQ(expected_start, buffer.start);
    EXPECT_EQ(expected_pos, buffer.pos);
    EXPECT_EQ(expected_end, buffer.end);
}
