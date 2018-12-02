#include "test_helpers.h"

static void expect_encode_decode_encode_binary_data(int length, std::vector<uint8_t> expected_prefix)
{
    std::vector<uint8_t> entities = make_values_of_length<uint8_t>(length);
    std::vector<uint8_t> expected_memory(expected_prefix);
    expected_memory.insert(expected_memory.end(), entities.begin(), entities.end());
    expect_memory_after_add_value(entities, expected_memory);
    expect_decode_encode(expected_memory);
}

static void expect_add_binary_data_incomplete(int byte_length, int array_length)
{
    std::vector<uint8_t> entities = make_values_of_length<uint8_t>(array_length);
    expect_add_value_incomplete(byte_length, entities);
}

#define DEFINE_BINARY_DATA_TEST(NAME, LENGTH, ...) \
TEST(NAME, length_ ## LENGTH) \
{ \
    expect_encode_decode_encode_binary_data(LENGTH, __VA_ARGS__); \
}

DEFINE_BINARY_DATA_TEST(BinaryData, 0, {0x91, 0})
DEFINE_BINARY_DATA_TEST(BinaryData, 1, {0x91, 4})
DEFINE_BINARY_DATA_TEST(BinaryData, 2, {0x91, 8})

TEST(BinaryData, incomplete)
{
    int byte_length = 10;
    expect_add_binary_data_incomplete(byte_length - 1, byte_length);
}
