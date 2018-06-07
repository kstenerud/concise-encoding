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

static void expect_memory_after_add_string_inferred_length(int length)
{
    std::string str = make_string_with_length(length);
    std::vector<uint8_t> expected_memory(str.c_str(), str.c_str() + str.size());
    uint8_t type = TYPE_STRING_0 + length;
    expected_memory.insert(expected_memory.begin(), type);
    expect_memory_after_add_value(str, expected_memory);
}

static void expect_memory_after_add_string(int length, std::vector<uint8_t> length_field_values)
{
    std::string str = make_string_with_length(length);
    std::vector<uint8_t> expected_memory(str.c_str(), str.c_str() + str.size());
    uint8_t type = TYPE_ARRAY_STRING;
    // TODO: Update this to use arrays
    expected_memory.insert(expected_memory.begin(), length_field_values.begin(), length_field_values.end());
    expected_memory.insert(expected_memory.begin(), type);
    expect_memory_after_add_value(str, expected_memory);
}

#define DEFINE_ADD_STRING_INFERRED_LENGTH_TEST(LENGTH) \
TEST(StringTest, inferred_length_ ## LENGTH) \
{ \
    expect_memory_after_add_string_inferred_length(LENGTH); \
}

#define DEFINE_ADD_STRING_TEST(LENGTH, ...) \
TEST(StringTest, length_ ## LENGTH) \
{ \
    expect_memory_after_add_string(LENGTH, __VA_ARGS__); \
}

DEFINE_ADD_STRING_INFERRED_LENGTH_TEST(0)
DEFINE_ADD_STRING_INFERRED_LENGTH_TEST(1)
DEFINE_ADD_STRING_INFERRED_LENGTH_TEST(2)
DEFINE_ADD_STRING_INFERRED_LENGTH_TEST(3)
DEFINE_ADD_STRING_INFERRED_LENGTH_TEST(4)
DEFINE_ADD_STRING_INFERRED_LENGTH_TEST(5)
DEFINE_ADD_STRING_INFERRED_LENGTH_TEST(6)
DEFINE_ADD_STRING_INFERRED_LENGTH_TEST(7)
DEFINE_ADD_STRING_INFERRED_LENGTH_TEST(8)
DEFINE_ADD_STRING_INFERRED_LENGTH_TEST(9)
DEFINE_ADD_STRING_INFERRED_LENGTH_TEST(10)
DEFINE_ADD_STRING_INFERRED_LENGTH_TEST(11)
DEFINE_ADD_STRING_INFERRED_LENGTH_TEST(12)
DEFINE_ADD_STRING_INFERRED_LENGTH_TEST(13)
DEFINE_ADD_STRING_INFERRED_LENGTH_TEST(14)
DEFINE_ADD_STRING_INFERRED_LENGTH_TEST(15)

DEFINE_ADD_STRING_TEST(    16, {0x40});
DEFINE_ADD_STRING_TEST(    17, {0x44});
DEFINE_ADD_STRING_TEST(    62, {0xf8});
DEFINE_ADD_STRING_TEST(    63, {0xfc});
DEFINE_ADD_STRING_TEST(    64, {0x01, 0x01});
DEFINE_ADD_STRING_TEST(    65, {0x05, 0x01});
DEFINE_ADD_STRING_TEST(0x3fff, {0xfd, 0xff});
DEFINE_ADD_STRING_TEST(0x4000, {0x02, 0x00, 0x01, 0x00});
