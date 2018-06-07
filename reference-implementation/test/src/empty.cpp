#include "test_helpers.h"

TEST(EmptyTest, empty)
{
    std::vector<uint8_t> expected_memory = {TYPE_EMPTY};

    expect_memory_after_operation([](cbe_buffer* buffer) {
        return cbe_add_empty(buffer);
    }, expected_memory);
    expect_decode_encode(expected_memory);
}
