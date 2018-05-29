#include "test_helpers.h"

static std::string make_string_with_length(int length)
{
	std::stringstream stream;
	static const char characters[] =
	{
		'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
		'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
		'0','1','2','3','4','5','6','7','8','9'
	};
	static const int character_count = sizeof(characters) / sizeof(*characters);
	for(int i = 0; i < length; i++)
	{
		stream << characters[i % character_count];
	}
	return stream.str();
}

static void test_inferred_string_write(int length)
{
	std::string str = make_string_with_length(length);
	std::vector<uint8_t> expected_memory(str.c_str(), str.c_str() + str.size());
	uint8_t type = TYPE_STRING_0 + length;
	expected_memory.insert(expected_memory.begin(), type);
	expect_memory_after_write(str, expected_memory);
}

static void test_string_write(int length, std::vector<uint8_t> length_field_values)
{
	std::string str = make_string_with_length(length);
	std::vector<uint8_t> expected_memory(str.c_str(), str.c_str() + str.size());
	uint8_t type = TYPE_STRING;
	expected_memory.insert(expected_memory.begin(), length_field_values.begin(), length_field_values.end());
	expected_memory.insert(expected_memory.begin(), type);
	expect_memory_after_write(str, expected_memory);
}

#define DEFINE_INFERRED_STRING_WRITE_TEST(LENGTH) \
TEST(StringTest, inferred_length_ ## LENGTH) \
{ \
    test_inferred_string_write(LENGTH); \
}

#define DEFINE_STRING_WRITE_TEST(LENGTH, ...) \
TEST(StringTest, length_ ## LENGTH) \
{ \
    test_string_write(LENGTH, __VA_ARGS__); \
}

DEFINE_INFERRED_STRING_WRITE_TEST(0)
DEFINE_INFERRED_STRING_WRITE_TEST(1)
DEFINE_INFERRED_STRING_WRITE_TEST(2)
DEFINE_INFERRED_STRING_WRITE_TEST(3)
DEFINE_INFERRED_STRING_WRITE_TEST(4)
DEFINE_INFERRED_STRING_WRITE_TEST(5)
DEFINE_INFERRED_STRING_WRITE_TEST(6)
DEFINE_INFERRED_STRING_WRITE_TEST(7)
DEFINE_INFERRED_STRING_WRITE_TEST(8)
DEFINE_INFERRED_STRING_WRITE_TEST(9)
DEFINE_INFERRED_STRING_WRITE_TEST(10)
DEFINE_INFERRED_STRING_WRITE_TEST(11)
DEFINE_INFERRED_STRING_WRITE_TEST(12)
DEFINE_INFERRED_STRING_WRITE_TEST(13)
DEFINE_INFERRED_STRING_WRITE_TEST(14)
DEFINE_INFERRED_STRING_WRITE_TEST(15)

DEFINE_STRING_WRITE_TEST( 16, {16});
DEFINE_STRING_WRITE_TEST( 17, {17});
DEFINE_STRING_WRITE_TEST(251, {251});
DEFINE_STRING_WRITE_TEST(252, {252});
DEFINE_STRING_WRITE_TEST(253, {LENGTH_16BIT, 0xfd, 0x00});
DEFINE_STRING_WRITE_TEST(254, {LENGTH_16BIT, 0xfe, 0x00});
DEFINE_STRING_WRITE_TEST(255, {LENGTH_16BIT, 0xff, 0x00});
DEFINE_STRING_WRITE_TEST(256, {LENGTH_16BIT, 0x00, 0x01});
DEFINE_STRING_WRITE_TEST(65535, {LENGTH_16BIT, 0xff, 0xff});
DEFINE_STRING_WRITE_TEST(65536, {LENGTH_32BIT, 0x00, 0x00, 0x01, 0x00});
