#include "helpers/test_helpers.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"

using namespace enc;

TEST_ENCODE_DECODE_SHRINKING(String,   size_0, 1, str(make_string(0)),  {0x80})
TEST_ENCODE_DECODE_SHRINKING(String,   size_1, 1, str(make_string(1)),  concat({0x81}, as_vector(make_string(1))))
TEST_ENCODE_DECODE_SHRINKING(String,   size_2, 1, str(make_string(2)),  concat({0x82}, as_vector(make_string(2))))
TEST_ENCODE_DECODE_SHRINKING(String,   size_3, 1, str(make_string(3)),  concat({0x83}, as_vector(make_string(3))))
TEST_ENCODE_DECODE_SHRINKING(String,   size_4, 1, str(make_string(4)),  concat({0x84}, as_vector(make_string(4))))
TEST_ENCODE_DECODE_SHRINKING(String,   size_5, 1, str(make_string(5)),  concat({0x85}, as_vector(make_string(5))))
TEST_ENCODE_DECODE_SHRINKING(String,   size_6, 1, str(make_string(6)),  concat({0x86}, as_vector(make_string(6))))
TEST_ENCODE_DECODE_SHRINKING(String,   size_7, 1, str(make_string(7)),  concat({0x87}, as_vector(make_string(7))))
TEST_ENCODE_DECODE_SHRINKING(String,   size_8, 1, str(make_string(8)),  concat({0x88}, as_vector(make_string(8))))
TEST_ENCODE_DECODE_SHRINKING(String,   size_9, 1, str(make_string(9)),  concat({0x89}, as_vector(make_string(9))))
TEST_ENCODE_DECODE_SHRINKING(String,  size_10, 1, str(make_string(10)), concat({0x8a}, as_vector(make_string(10))))
TEST_ENCODE_DECODE_SHRINKING(String,  size_11, 1, str(make_string(11)), concat({0x8b}, as_vector(make_string(11))))
TEST_ENCODE_DECODE_SHRINKING(String,  size_12, 1, str(make_string(12)), concat({0x8c}, as_vector(make_string(12))))
TEST_ENCODE_DECODE_SHRINKING(String,  size_13, 1, str(make_string(13)), concat({0x8d}, as_vector(make_string(13))))
TEST_ENCODE_DECODE_SHRINKING(String,  size_14, 1, str(make_string(14)), concat({0x8e}, as_vector(make_string(14))))
TEST_ENCODE_DECODE_SHRINKING(String,  size_15, 1, str(make_string(15)), concat({0x8f}, as_vector(make_string(15))))
TEST_ENCODE_DECODE_SHRINKING(String,  size_16, 2, str(make_string(16)), concat({0x90}, array_length_field(16), as_vector(make_string(16))))
TEST_ENCODE_DECODE_SHRINKING(String, size_500, 3, str(make_string(500)), concat({0x90}, array_length_field(500), as_vector(make_string(500))))

TEST_ENCODE_STATUS(String, too_long,   99, 9, CBE_ENCODE_ERROR_ARRAY_FIELD_LENGTH_EXCEEDED, strh(1)->data({0x30, 0x30}))
TEST_ENCODE_STATUS(String, too_short,  99, 9, CBE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD, strh(5)->data({0x30, 0x30}))
TEST_ENCODE_STATUS(String, int,        99, 9, CBE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD, strh(5)->data({0x30})->si(0x7ffffffffl, 0))
TEST_ENCODE_STATUS(String, float,      99, 9, CBE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD, strh(5)->data({0x30})->flt(0.1))
TEST_ENCODE_STATUS(String, decimal,    99, 9, CBE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD, strh(5)->data({0x30})->dec(0.1))
TEST_ENCODE_STATUS(String, time,       99, 9, CBE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD, strh(5)->data({0x30})->time(0))
TEST_ENCODE_STATUS(String, boolean,    99, 9, CBE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD, strh(5)->data({0x30})->bl(false))
TEST_ENCODE_STATUS(String, list,       99, 9, CBE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD, strh(5)->data({0x30})->list()->end())
TEST_ENCODE_STATUS(String, map,        99, 9, CBE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD, strh(5)->data({0x30})->map()->end())
TEST_ENCODE_STATUS(String, end,        99, 9, CBE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD, strh(5)->data({0x30})->end())
TEST_ENCODE_STATUS(String, nil,        99, 9, CBE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD, strh(5)->data({0x30})->nil())
TEST_ENCODE_STATUS(String, padding,    99, 9, CBE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD, strh(5)->data({0x30})->pad(1))
TEST_ENCODE_STATUS(String, string,     99, 9, CBE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD, strh(5)->data({0x30})->str("a"))
TEST_ENCODE_STATUS(String, string16,   99, 9, CBE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD, strh(5)->data({0x30})->str("1234567890123456"))
TEST_ENCODE_STATUS(String, binary,     99, 9, CBE_ENCODE_ERROR_INCOMPLETE_ARRAY_FIELD, strh(5)->data({0x30})->bin({0x00}))

TEST_DECODE_STATUS(String, decode_too_short,  99, 9, true, CBE_DECODE_ERROR_INCOMPLETE_ARRAY_FIELD, {0x85, 0x30, 0x30})
TEST_DECODE_STATUS(String, decode_too_short2, 99, 9, true, CBE_DECODE_ERROR_INCOMPLETE_ARRAY_FIELD, {0x90, 0x40, 0x30})

TEST_ENCODE_STATUS(String, encode_bad_chars, 99, 9, CBE_ENCODE_ERROR_INVALID_ARRAY_DATA, str({0x40, (char)0x81, 0x42, 0x43, 0x44}))

TEST_DECODE_STATUS(String, decode_bad_character,  99, 9, true, CBE_DECODE_ERROR_INVALID_ARRAY_DATA, {0x89, 0x61, 0x80, 0x63, 0x6f, 0x6d, 0x6d, 0x65, 0x6e, 0x74})

TEST(String, encode_convenience_fail)
{
    const cbe_encode_status expected_status = CBE_ENCODE_STATUS_NEED_MORE_ROOM;
    std::vector<uint8_t> data = {0x40, 0x41, 0x42, 0x43, 0x44};
    const int64_t buffer_size = 4;
    const int max_container_depth = 100;
    cbe_encoder encoder(buffer_size, max_container_depth);
    cbe_encode_status status = encoder.encode_string(data);
    EXPECT_EQ(expected_status, status);
    EXPECT_EQ(0, encoder.get_encode_buffer_offset());
}

TEST(String, encode_convenience_fail_long)
{
    const cbe_encode_status expected_status = CBE_ENCODE_STATUS_NEED_MORE_ROOM;
    std::vector<uint8_t> data = {0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
                                 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
                                 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47};
    const int64_t buffer_size = 10;
    const int max_container_depth = 100;
    cbe_encoder encoder(buffer_size, max_container_depth);
    cbe_encode_status status = encoder.encode_string(data);
    EXPECT_EQ(expected_status, status);
    EXPECT_EQ(0, encoder.get_encode_buffer_offset());
}
