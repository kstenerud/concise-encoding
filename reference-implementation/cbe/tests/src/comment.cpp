#include "helpers/test_helpers.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"

using namespace enc;

static std::string g_bad_chars[] =
{
    /*"\u0000",*/ "\u0001", "\u0002", "\u0003", "\u0004", "\u0005", "\u0006", "\u0007",
    "\u0008", /*"\u0009",*/ "\u000a", "\u000b", "\u000c", "\u000d", "\u000e", "\u000f",
    "\u0010", "\u0011", "\u0012", "\u0013", "\u0014", "\u0015", "\u0016", "\u0017",
    "\u0018", "\u0019", "\u001a", "\u001b", "\u001c", "\u001d", "\u001e", "\u001f",
    "\u007f",
    "\u0080", "\u0081", "\u0082", "\u0083", "\u0084", "\u0085", "\u0086", "\u0087",
    "\u0088", "\u0089", "\u008a", "\u008b", "\u008c", "\u008d", "\u008e", "\u008f",
    "\u2028", // line separator
    "\u2029", // paragraph separator
};
static int g_bad_chars_count = sizeof(g_bad_chars) / sizeof(*g_bad_chars);


TEST_ENCODE_DECODE_SHRINKING(Comment,  size_0, 2, cmt(make_string(0)),  concat({0x93}, {0x00}))
TEST_ENCODE_DECODE_SHRINKING(Comment, size_16, 2, cmt(make_string(16)), concat({0x93}, {0x10}, as_vector(make_string(16))))

TEST_ENCODE_STATUS(Comment, encode_bad_data, 99, 9, CBE_ENCODE_ERROR_INVALID_ARRAY_DATA, cmt("Test\nblah"))
TEST_DECODE_STATUS(Comment, decode_bad_data, 99, 9, true, CBE_DECODE_ERROR_INVALID_ARRAY_DATA, {0x93, 0x18, 0x41, 0x0a, 0x74, 0x65, 0x73, 0x74})

TEST(Comment, encode_bad_chars)
{
    const int64_t buffer_size = 100;
    const int max_container_depth = 100;
    const cbe_encode_status expected_status = CBE_ENCODE_ERROR_INVALID_ARRAY_DATA;
    std::vector<uint8_t> data = {0x40, 0x41, 0x42, 0x00, 0x43, 0x44};
    cbe_encoder encoder(buffer_size, max_container_depth);
    cbe_encode_status status = encoder.encode_comment(data);
    EXPECT_EQ(expected_status, status);

    for(int i = 0; i < g_bad_chars_count; i++)
    {
        std::string str = std::string("test") + g_bad_chars[i] + std::string("blah");
        cbe_test::expect_encode_produces_status(buffer_size, max_container_depth, cmt(str), expected_status);
    }
}

TEST(Comment, decode_bad_character)
{
    const int64_t buffer_size = 100;
    const int max_container_depth = 100;
    const bool callback_return_value = true;
    const cbe_decode_status expected_status = CBE_DECODE_ERROR_INVALID_ARRAY_DATA;
    uint8_t data[] = {0x93, 0x24, 0x61, 0x00, 0x63, 0x6f, 0x6d, 0x6d, 0x65, 0x6e, 0x74};
    cbe_test::expect_decode_produces_status(buffer_size,
                                            max_container_depth,
                                            callback_return_value,
                                            std::vector<uint8_t>(data, data + sizeof(data)),
                                            expected_status);

    uint8_t header[] = {0x93, 0x00};
    for(int i = 0; i < g_bad_chars_count; i++)
    {
        std::string str = std::string("test") + g_bad_chars[i] + std::string("blah");
        std::vector<uint8_t> string_data((uint8_t*)str.data(), (uint8_t*)str.data() + str.size());
        header[1] = (uint8_t)str.size() << 2;
        string_data.insert(string_data.begin(), header, header + sizeof(header));
        cbe_test::expect_decode_produces_status(buffer_size,
                                            max_container_depth,
                                            callback_return_value,
                                            string_data,
                                            expected_status);
    }
}

TEST(Comment, encode_convenience_fail)
{
    const cbe_encode_status expected_status = CBE_ENCODE_STATUS_NEED_MORE_ROOM;
    std::vector<uint8_t> data = {0x40, 0x41, 0x42, 0x43, 0x44};
    const int64_t buffer_size = 4;
    const int max_container_depth = 100;
    cbe_encoder encoder(buffer_size, max_container_depth);
    cbe_encode_status status = encoder.encode_comment(data);
    EXPECT_EQ(expected_status, status);
    EXPECT_EQ(0, encoder.get_encode_buffer_offset());
}
