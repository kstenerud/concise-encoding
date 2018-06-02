#include "encode_test_helpers.h"

static void expect_memory_after_add_bytes_inferred_length(int length)
{
	std::vector<uint8_t> bytes = make_values_of_length<uint8_t>(length);
	std::vector<uint8_t> expected_memory(bytes);
	uint8_t type = TYPE_BYTES_0 + length;
	expected_memory.insert(expected_memory.begin(), type);
	expect_memory_after_add_value(bytes, expected_memory);
}

static void expect_memory_after_add_bytes(int length, std::vector<uint8_t> length_field_values)
{
	std::vector<uint8_t> bytes = make_values_of_length<uint8_t>(length);
	std::vector<uint8_t> expected_memory(bytes);
	uint8_t type = TYPE_BYTES;
	expected_memory.insert(expected_memory.begin(), length_field_values.begin(), length_field_values.end());
	expected_memory.insert(expected_memory.begin(), type);
	expect_memory_after_add_value(bytes, expected_memory);
}

#define DEFINE_ADD_BYTES_INFERRED_LENGTH_TEST(LENGTH) \
TEST(BytesTest, inferred_length_ ## LENGTH) \
{ \
    expect_memory_after_add_bytes_inferred_length(LENGTH); \
}

#define DEFINE_ADD_BYTES_TEST(LENGTH, ...) \
TEST(BytesTest, length_ ## LENGTH) \
{ \
    expect_memory_after_add_bytes(LENGTH, __VA_ARGS__); \
}

DEFINE_ADD_BYTES_INFERRED_LENGTH_TEST(0)
DEFINE_ADD_BYTES_INFERRED_LENGTH_TEST(1)
DEFINE_ADD_BYTES_INFERRED_LENGTH_TEST(2)
DEFINE_ADD_BYTES_INFERRED_LENGTH_TEST(3)
DEFINE_ADD_BYTES_INFERRED_LENGTH_TEST(4)
DEFINE_ADD_BYTES_INFERRED_LENGTH_TEST(5)
DEFINE_ADD_BYTES_INFERRED_LENGTH_TEST(6)
DEFINE_ADD_BYTES_INFERRED_LENGTH_TEST(7)
DEFINE_ADD_BYTES_INFERRED_LENGTH_TEST(8)
DEFINE_ADD_BYTES_INFERRED_LENGTH_TEST(9)
DEFINE_ADD_BYTES_INFERRED_LENGTH_TEST(10)
DEFINE_ADD_BYTES_INFERRED_LENGTH_TEST(11)
DEFINE_ADD_BYTES_INFERRED_LENGTH_TEST(12)
DEFINE_ADD_BYTES_INFERRED_LENGTH_TEST(13)
DEFINE_ADD_BYTES_INFERRED_LENGTH_TEST(14)
DEFINE_ADD_BYTES_INFERRED_LENGTH_TEST(15)

DEFINE_ADD_BYTES_TEST( 16, {16});
DEFINE_ADD_BYTES_TEST( 17, {17});
DEFINE_ADD_BYTES_TEST(251, {251});
DEFINE_ADD_BYTES_TEST(252, {252});
DEFINE_ADD_BYTES_TEST(253, {BYTES_LENGTH_16_BIT, 0xfd, 0x00});
DEFINE_ADD_BYTES_TEST(254, {BYTES_LENGTH_16_BIT, 0xfe, 0x00});
DEFINE_ADD_BYTES_TEST(255, {BYTES_LENGTH_16_BIT, 0xff, 0x00});
DEFINE_ADD_BYTES_TEST(256, {BYTES_LENGTH_16_BIT, 0x00, 0x01});
DEFINE_ADD_BYTES_TEST(65535, {BYTES_LENGTH_16_BIT, 0xff, 0xff});
DEFINE_ADD_BYTES_TEST(65536, {BYTES_LENGTH_32_BIT, 0x00, 0x00, 0x01, 0x00});
