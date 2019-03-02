#include "helpers/test_helpers.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"

using namespace enc;

TEST_ENCODE_DECODE_SHRINKING(Binary,  size_0,   2, bin(make_bytes(0)), concat({0x91}, array_length_field(0)))
TEST_ENCODE_DECODE_SHRINKING(Binary,  size_1,   2, bin(make_bytes(1)), concat({0x91}, array_length_field(1), make_bytes(1)))
TEST_ENCODE_DECODE_SHRINKING(Binary,  size_2,   2, bin(make_bytes(2)), concat({0x91}, array_length_field(2), make_bytes(2)))
TEST_ENCODE_DECODE_SHRINKING(Binary,  size_500, 3, bin(make_bytes(500)), concat({0x91}, array_length_field(500), make_bytes(500)))

TEST_ENCODE_STATUS(Binary, too_long,   99, 9, CBE_ENCODE_ERROR_FIELD_LENGTH_EXCEEDED, binh(1)->data({0x30, 0x30}));
TEST_ENCODE_STATUS(Binary, too_short,  99, 9, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30, 0x30}));
TEST_ENCODE_STATUS(Binary, int8,       99, 9, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->i8(0));
TEST_ENCODE_STATUS(Binary, int16,      99, 9, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->i16(0x7ff));
TEST_ENCODE_STATUS(Binary, int32,      99, 9, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->i32(0x7ffff));
TEST_ENCODE_STATUS(Binary, int64,      99, 9, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->i64(0x7ffffffffl));
TEST_ENCODE_STATUS(Binary, int128,     99, 9, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->i128(0x7ffffffffl, 0));
TEST_ENCODE_STATUS(Binary, float32,    99, 9, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->f32(0.1));
TEST_ENCODE_STATUS(Binary, float64,    99, 9, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->f64(0.1));
TEST_ENCODE_STATUS(Binary, float128,   99, 9, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->f128(0.1));
TEST_ENCODE_STATUS(Binary, decimal32,  99, 9, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->d32(0.1));
TEST_ENCODE_STATUS(Binary, decimal64,  99, 9, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->d64(0.1));
TEST_ENCODE_STATUS(Binary, decimal128, 99, 9, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->d128(0.1));
TEST_ENCODE_STATUS(Binary, time,       99, 9, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->smtime(0));
TEST_ENCODE_STATUS(Binary, boolean,    99, 9, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->bl(false));
TEST_ENCODE_STATUS(Binary, list,       99, 9, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->list()->end());
TEST_ENCODE_STATUS(Binary, map,        99, 9, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->map()->end());
TEST_ENCODE_STATUS(Binary, end,        99, 9, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->end());
TEST_ENCODE_STATUS(Binary, nil,        99, 9, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->nil());
TEST_ENCODE_STATUS(Binary, padding,    99, 9, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->pad(1));
TEST_ENCODE_STATUS(Binary, string,     99, 9, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->str("a"));
TEST_ENCODE_STATUS(Binary, string16,   99, 9, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->str("1234567890123456"));
TEST_ENCODE_STATUS(Binary, binary,     99, 9, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->bin({0x00}));

TEST_DECODE_STATUS(Binary, decode_too_short, 99, 9, true, CBE_DECODE_ERROR_INCOMPLETE_FIELD, {0x91, 0x40, 0x30});

TEST(Binary, encode_convenience)
{
    const cbe_encode_status expected_status = CBE_ENCODE_STATUS_OK;
    std::vector<uint8_t> data = {0x40, 0x81, 0x42, 0x43, 0x44};
    const int64_t buffer_size = 100;
    const int max_container_depth = 100;
    cbe_encoder encoder(buffer_size, max_container_depth);
    cbe_encode_status status = encoder.encode_binary(data);
    EXPECT_EQ(expected_status, status);
}

TEST(Binary, encode_convenience_fail)
{
    const cbe_encode_status expected_status = CBE_ENCODE_STATUS_NEED_MORE_ROOM;
    std::vector<uint8_t> data = {0x40, 0x81, 0x42, 0x43, 0x44};
    const int64_t buffer_size = 4;
    const int max_container_depth = 100;
    cbe_encoder encoder(buffer_size, max_container_depth);
    cbe_encode_status status = encoder.encode_binary(data);
    EXPECT_EQ(expected_status, status);
    EXPECT_EQ(0, encoder.get_encode_buffer_offset());
}
