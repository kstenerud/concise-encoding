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

static void expect_memory_string_inferred_length(int length)
{
    std::string str = make_string_with_length(length);
    std::vector<uint8_t> expected_memory(str.c_str(), str.c_str() + str.size());
    uint8_t type = 0x80 + length;
    expected_memory.insert(expected_memory.begin(), type);
    expect_memory_after_add_value(str, expected_memory);
    expect_decode_encode(expected_memory);
}

static void expect_memory_string(int length, std::vector<uint8_t> length_field_values)
{
    std::string str = make_string_with_length(length);
    std::vector<uint8_t> expected_memory(str.c_str(), str.c_str() + str.size());
    uint8_t type = 0x90;
    expected_memory.insert(expected_memory.begin(), length_field_values.begin(), length_field_values.end());
    expected_memory.insert(expected_memory.begin(), type);
    expect_memory_after_add_value(str, expected_memory);
    expect_decode_encode(expected_memory);
}

static void expect_incomplete_add_string(int buffer_length, int string_length)
{
    std::string str = make_string_with_length(string_length);
    expect_add_value_incomplete(buffer_length, str);
}

#define DEFINE_ADD_STRING_INFERRED_LENGTH_TEST(LENGTH) \
TEST(StringTest, inferred_length_ ## LENGTH) \
{ \
    expect_memory_string_inferred_length(LENGTH); \
}

#define DEFINE_ADD_STRING_TEST(LENGTH, ...) \
TEST(StringTest, length_ ## LENGTH) \
{ \
    expect_memory_string(LENGTH, __VA_ARGS__); \
}

#define DEFINE_ADD_STRING_INCOMPLETE_TEST(SIZE) \
TEST(StringTest, incomplete_length_ ## SIZE) \
{ \
    expect_incomplete_add_string(SIZE - 1, SIZE); \
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

DEFINE_ADD_STRING_INCOMPLETE_TEST(1)
DEFINE_ADD_STRING_INCOMPLETE_TEST(2)
DEFINE_ADD_STRING_INCOMPLETE_TEST(3)
DEFINE_ADD_STRING_INCOMPLETE_TEST(4)
DEFINE_ADD_STRING_INCOMPLETE_TEST(5)
DEFINE_ADD_STRING_INCOMPLETE_TEST(6)
DEFINE_ADD_STRING_INCOMPLETE_TEST(7)
DEFINE_ADD_STRING_INCOMPLETE_TEST(8)
DEFINE_ADD_STRING_INCOMPLETE_TEST(9)
DEFINE_ADD_STRING_INCOMPLETE_TEST(10)
DEFINE_ADD_STRING_INCOMPLETE_TEST(11)
DEFINE_ADD_STRING_INCOMPLETE_TEST(12)
DEFINE_ADD_STRING_INCOMPLETE_TEST(13)
DEFINE_ADD_STRING_INCOMPLETE_TEST(14)
DEFINE_ADD_STRING_INCOMPLETE_TEST(15)
DEFINE_ADD_STRING_INCOMPLETE_TEST(16)
DEFINE_ADD_STRING_INCOMPLETE_TEST(17)
DEFINE_ADD_STRING_INCOMPLETE_TEST(0x1000)


static std::vector<uint8_t> generate_length_field(int64_t length)
{
    int64_t length_bytes = length << 2;
    uint8_t* bytes = (uint8_t*)&length_bytes;
    if(length > 0x3fffffff)
    {
        length_bytes += 3;
        return std::vector<uint8_t>(bytes, bytes + 8);
    }
    if(length > 0x3fff)
    {
        length_bytes += 2;
        return std::vector<uint8_t>(bytes, bytes + 4);
    }
    if(length > 0x3f)
    {
        length_bytes += 1;
        return std::vector<uint8_t>(bytes, bytes + 2);
    }
    return std::vector<uint8_t>(bytes, bytes + 1);
}

TEST(Strings, multipart)
{
    int length = 1000;
    std::vector<uint8_t> length_field_values = generate_length_field(length);
    std::string str = make_string_with_length(length);
    std::vector<uint8_t> expected_memory(str.c_str(), str.c_str() + str.size());
    uint8_t type = 0x90;
    expected_memory.insert(expected_memory.begin(), length_field_values.begin(), length_field_values.end());
    expected_memory.insert(expected_memory.begin(), type);
    expect_memory_after_add_value(str, expected_memory);
    expect_decode_encode(expected_memory);
}

TEST(Strings, encode_multipart)
{
    uint8_t document[100];
    int field_length = 10;
    uint8_t data[5];
    struct cbe_encode_process* process = cbe_encode_begin(document, sizeof(document));
    EXPECT_ENCODE_OK(cbe_encode_begin_list(process));
        EXPECT_ENCODE_OK(cbe_encode_begin_string(process, field_length));
        EXPECT_ENCODE_OK(cbe_encode_add_data(process, data, sizeof(data)));
        EXPECT_ENCODE_OK(cbe_encode_add_data(process, data, sizeof(data)));
    EXPECT_ENCODE_OK(cbe_encode_end_container(process));
    EXPECT_ENCODE_OK(cbe_encode_end(process));
}

TEST(Strings, encode_multipart_b)
{
    uint8_t document[100];
    int field_length = 10;
    uint8_t data[5];
    struct cbe_encode_process* process = cbe_encode_begin(document, sizeof(document));
    EXPECT_ENCODE_OK(cbe_encode_begin_list(process));
        EXPECT_ENCODE_OK(cbe_encode_begin_string(process, field_length));
        EXPECT_ENCODE_OK(cbe_encode_add_data(process, data, sizeof(data)));
        EXPECT_ENCODE_OK(cbe_encode_add_data(process, data, sizeof(data)));
        EXPECT_ENCODE_OK(cbe_encode_add_string(process, "test"));
    EXPECT_ENCODE_OK(cbe_encode_end_container(process));
    EXPECT_ENCODE_OK(cbe_encode_end(process));
}

TEST(Strings, encode_unfinished_field)
{
    uint8_t document[100];
    int field_length = 10;
    uint8_t data[5];
    struct cbe_encode_process* process = cbe_encode_begin(document, sizeof(document));
    EXPECT_ENCODE_OK(cbe_encode_begin_list(process));
        EXPECT_ENCODE_OK(cbe_encode_begin_string(process, field_length));
        EXPECT_ENCODE_OK(cbe_encode_add_data(process, data, sizeof(data)));
    EXPECT_EQ(CBE_ENCODE_ERROR_INCOMPLETE_FIELD, cbe_encode_end_container(process));
    cbe_encode_end(process);
}

TEST(Strings, encode_unfinished_field_b)
{
    uint8_t document[100];
    int field_length = 10;
    uint8_t data[5];
    __int128 value128 = 0x1000000000;
    value128 *= value128;
    struct cbe_encode_process* process = cbe_encode_begin(document, sizeof(document));
    EXPECT_ENCODE_OK(cbe_encode_begin_list(process));
        EXPECT_ENCODE_OK(cbe_encode_begin_string(process, field_length));
        EXPECT_ENCODE_OK(cbe_encode_add_data(process, data, sizeof(data)));
        EXPECT_EQ(CBE_ENCODE_ERROR_INCOMPLETE_FIELD, cbe_encode_add_empty(process));
        EXPECT_EQ(CBE_ENCODE_ERROR_INCOMPLETE_FIELD, cbe_encode_add_boolean(process, true));
        EXPECT_EQ(CBE_ENCODE_ERROR_INCOMPLETE_FIELD, cbe_encode_add_int(process, 1));
        EXPECT_EQ(CBE_ENCODE_ERROR_INCOMPLETE_FIELD, cbe_encode_add_int_8(process, 1));
        EXPECT_EQ(CBE_ENCODE_ERROR_INCOMPLETE_FIELD, cbe_encode_add_int_16(process, 0x100));
        EXPECT_EQ(CBE_ENCODE_ERROR_INCOMPLETE_FIELD, cbe_encode_add_int_32(process, 0x10000));
        EXPECT_EQ(CBE_ENCODE_ERROR_INCOMPLETE_FIELD, cbe_encode_add_int_64(process, 0x1000000000));
        EXPECT_EQ(CBE_ENCODE_ERROR_INCOMPLETE_FIELD, cbe_encode_add_int_128(process, value128));
        EXPECT_EQ(CBE_ENCODE_ERROR_INCOMPLETE_FIELD, cbe_encode_add_float_32(process, 0.1f));
        EXPECT_EQ(CBE_ENCODE_ERROR_INCOMPLETE_FIELD, cbe_encode_add_float_64(process, 0.1));
        EXPECT_EQ(CBE_ENCODE_ERROR_INCOMPLETE_FIELD, cbe_encode_add_decimal_32(process, 0.1df));
        EXPECT_EQ(CBE_ENCODE_ERROR_INCOMPLETE_FIELD, cbe_encode_add_decimal_64(process, 0.1dd));
        EXPECT_EQ(CBE_ENCODE_ERROR_INCOMPLETE_FIELD, cbe_encode_add_time(process, 1));
        EXPECT_EQ(CBE_ENCODE_ERROR_INCOMPLETE_FIELD, cbe_encode_begin_list(process));
        EXPECT_EQ(CBE_ENCODE_ERROR_INCOMPLETE_FIELD, cbe_encode_begin_map(process));
        EXPECT_EQ(CBE_ENCODE_ERROR_INCOMPLETE_FIELD, cbe_encode_end_container(process));
    cbe_encode_end(process);
}

TEST(Strings, encode_unfinished_field_two_part)
{
    uint8_t document[100];
    int field_length = 10;
    uint8_t data[3];
    struct cbe_encode_process* process = cbe_encode_begin(document, sizeof(document));
    EXPECT_ENCODE_OK(cbe_encode_begin_list(process));
        EXPECT_ENCODE_OK(cbe_encode_begin_string(process, field_length));
        EXPECT_ENCODE_OK(cbe_encode_add_data(process, data, sizeof(data)));
        EXPECT_ENCODE_OK(cbe_encode_add_data(process, data, sizeof(data)));
    EXPECT_EQ(CBE_ENCODE_ERROR_INCOMPLETE_FIELD, cbe_encode_end_container(process));
    cbe_encode_end(process);
}

TEST(Strings, encode_too_big_field)
{
    uint8_t document[100];
    int field_length = 10;
    uint8_t data[50];
    struct cbe_encode_process* process = cbe_encode_begin(document, sizeof(document));
    EXPECT_ENCODE_OK(cbe_encode_begin_list(process));
        EXPECT_ENCODE_OK(cbe_encode_begin_string(process, field_length));
        EXPECT_EQ(CBE_ENCODE_ERROR_FIELD_LENGTH_EXCEEDED, cbe_encode_add_data(process, data, sizeof(data)));
    cbe_encode_end(process);
}

TEST(Strings, encode_too_big_field_two_part)
{
    uint8_t document[100];
    int field_length = 10;
    uint8_t data[7];
    struct cbe_encode_process* process = cbe_encode_begin(document, sizeof(document));
    EXPECT_ENCODE_OK(cbe_encode_begin_list(process));
        EXPECT_ENCODE_OK(cbe_encode_begin_string(process, field_length));
        EXPECT_ENCODE_OK(cbe_encode_add_data(process, data, sizeof(data)));
        EXPECT_EQ(CBE_ENCODE_ERROR_FIELD_LENGTH_EXCEEDED, cbe_encode_add_data(process, data, sizeof(data)));
    cbe_encode_end(process);
}
