#include "test_helpers.h"

TEST(EmptyTest, empty)
{
    std::vector<uint8_t> expected_memory = {0x94};

    expect_memory_after_operation([](cbe_encode_process* encode_process) {
        return cbe_encode_add_empty(encode_process);
    }, expected_memory);
    expect_decode_encode(expected_memory);
}

TEST(EmptyTest, incomplete)
{
    expect_incomplete_operation(0, [](cbe_encode_process* encode_process) {
        return cbe_encode_add_empty(encode_process);
    });
}
