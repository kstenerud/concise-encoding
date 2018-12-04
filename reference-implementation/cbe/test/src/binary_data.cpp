#include "test_helpers.h"

static std::vector<uint8_t> make_bytes(int length)
{
    return make_values_of_length<uint8_t>(length);
}

static void expect_encode_decode_encode_binary_data(int length, std::vector<uint8_t> expected_prefix)
{
    std::vector<uint8_t> bytes = make_bytes(length);
    std::vector<uint8_t> expected_memory(expected_prefix);
    expected_memory.insert(expected_memory.end(), bytes.begin(), bytes.end());
    expect_memory_after_add_value(bytes, expected_memory);
    expect_decode_encode(expected_memory);
}

static void expect_add_binary_data_incomplete(int byte_length, int array_length)
{
    std::vector<uint8_t> bytes = make_bytes(array_length);
    expect_add_value_incomplete(byte_length, bytes);
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

TEST(BinaryData, encode_multipart)
{
    uint8_t document[100];
    int field_length = 10;
    uint8_t data[5];
    struct cbe_encode_process* process = cbe_encode_begin(document, sizeof(document));
    EXPECT_ENCODE_OK(cbe_encode_begin_list(process));
        EXPECT_ENCODE_OK(cbe_encode_begin_binary(process, field_length));
        EXPECT_ENCODE_OK(cbe_encode_add_data(process, data, sizeof(data)));
        EXPECT_ENCODE_OK(cbe_encode_add_data(process, data, sizeof(data)));
    EXPECT_ENCODE_OK(cbe_encode_end_container(process));
    EXPECT_ENCODE_OK(cbe_encode_end(process));
}

TEST(BinaryData, encode_multipart_b)
{
    uint8_t document[100];
    int field_length = 10;
    uint8_t data[5];
    struct cbe_encode_process* process = cbe_encode_begin(document, sizeof(document));
    EXPECT_ENCODE_OK(cbe_encode_begin_list(process));
        EXPECT_ENCODE_OK(cbe_encode_begin_binary(process, field_length));
        EXPECT_ENCODE_OK(cbe_encode_add_data(process, data, sizeof(data)));
        EXPECT_ENCODE_OK(cbe_encode_add_data(process, data, sizeof(data)));
        EXPECT_ENCODE_OK(cbe_encode_add_string(process, "test"));
    EXPECT_ENCODE_OK(cbe_encode_end_container(process));
    EXPECT_ENCODE_OK(cbe_encode_end(process));
}

TEST(BinaryData, encode_unfinished_field)
{
    uint8_t document[100];
    int field_length = 10;
    uint8_t data[5];
    struct cbe_encode_process* process = cbe_encode_begin(document, sizeof(document));
    EXPECT_ENCODE_OK(cbe_encode_begin_list(process));
        EXPECT_ENCODE_OK(cbe_encode_begin_binary(process, field_length));
        EXPECT_ENCODE_OK(cbe_encode_add_data(process, data, sizeof(data)));
    EXPECT_EQ(CBE_ENCODE_ERROR_INCOMPLETE_FIELD, cbe_encode_end_container(process));
    cbe_encode_end(process);
}

TEST(BinaryData, encode_unfinished_field_b)
{
    uint8_t document[100];
    int field_length = 10;
    uint8_t data[5];
    __int128 value128 = 0x1000000000;
    value128 *= value128;
    struct cbe_encode_process* process = cbe_encode_begin(document, sizeof(document));
    EXPECT_ENCODE_OK(cbe_encode_begin_list(process));
        EXPECT_ENCODE_OK(cbe_encode_begin_binary(process, field_length));
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

TEST(BinaryData, encode_unfinished_field_two_part)
{
    uint8_t document[100];
    int field_length = 10;
    uint8_t data[3];
    struct cbe_encode_process* process = cbe_encode_begin(document, sizeof(document));
    EXPECT_ENCODE_OK(cbe_encode_begin_list(process));
        EXPECT_ENCODE_OK(cbe_encode_begin_binary(process, field_length));
        EXPECT_ENCODE_OK(cbe_encode_add_data(process, data, sizeof(data)));
        EXPECT_ENCODE_OK(cbe_encode_add_data(process, data, sizeof(data)));
    EXPECT_EQ(CBE_ENCODE_ERROR_INCOMPLETE_FIELD, cbe_encode_end_container(process));
    cbe_encode_end(process);
}

TEST(BinaryData, encode_too_big_field)
{
    uint8_t document[100];
    int field_length = 10;
    uint8_t data[50];
    struct cbe_encode_process* process = cbe_encode_begin(document, sizeof(document));
    EXPECT_ENCODE_OK(cbe_encode_begin_list(process));
        EXPECT_ENCODE_OK(cbe_encode_begin_binary(process, field_length));
        EXPECT_EQ(CBE_ENCODE_ERROR_FIELD_LENGTH_EXCEEDED, cbe_encode_add_data(process, data, sizeof(data)));
    cbe_encode_end(process);
}

TEST(BinaryData, encode_too_big_field_two_part)
{
    uint8_t document[100];
    int field_length = 10;
    uint8_t data[7];
    struct cbe_encode_process* process = cbe_encode_begin(document, sizeof(document));
    EXPECT_ENCODE_OK(cbe_encode_begin_list(process));
        EXPECT_ENCODE_OK(cbe_encode_begin_binary(process, field_length));
        EXPECT_ENCODE_OK(cbe_encode_add_data(process, data, sizeof(data)));
        EXPECT_EQ(CBE_ENCODE_ERROR_FIELD_LENGTH_EXCEEDED, cbe_encode_add_data(process, data, sizeof(data)));
    cbe_encode_end(process);
}
