#include "test_helpers.h"

TEST(EmptyTest, empty)
{
    std::vector<uint8_t> expected_memory = {0x94};

    expect_memory_after_operation([](cbe_buffer* buffer) {
        return cbe_add_empty(buffer);
    }, expected_memory);
    expect_decode_encode(expected_memory);
}

TEST(EmptyTest, failed)
{
    expect_failed_operation(0, [](cbe_buffer* buffer) {
        return cbe_add_empty(buffer);
    });
}
