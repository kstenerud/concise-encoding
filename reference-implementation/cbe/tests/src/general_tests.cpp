#include "test_helpers.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"

#define MAX_CONTAINER_DEPTH 500

using namespace enc;

#define TEST_ENCODE_DECODE_SHRINKING_TIME(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, USEC, ...) \
TEST(Time, time_ ## YEAR ## _ ## MONTH ## _ ## DAY ## _ ## HOUR ## _ ## MINUTE ## _ ## SECOND ## _ ## USEC) \
{ \
	cbe_test::expect_encode_decode_with_shrinking_buffer_size(0, \
		smtime(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, USEC), \
		 __VA_ARGS__); \
}


#define TEST_ENCODE_DECODE_SHRINKING_INT128(NAME, HIGH, LOW, ...) \
TEST_ENCODE_DECODE_SHRINKING(Int128, NAME, i128(HIGH, LOW),  __VA_ARGS__) \

#define TEST_ENCODE_DECODE_SHRINKING_INT128_SIMPLE(NAME, VALUE, ...) \
TEST_ENCODE_DECODE_SHRINKING(Int128, NAME, i128(VALUE),  __VA_ARGS__) \

#define TEST_ENCODE_DECODE_SHRINKING_INT64(NAME, VALUE, ...) \
TEST_ENCODE_DECODE_SHRINKING(Int64, NAME, i64(VALUE),  __VA_ARGS__) \
TEST_ENCODE_DECODE_SHRINKING_INT128_SIMPLE(NAME, VALUE, __VA_ARGS__)

#define TEST_ENCODE_DECODE_SHRINKING_INT32(NAME, VALUE, ...) \
TEST_ENCODE_DECODE_SHRINKING(Int32,NAME, i32(VALUE),  __VA_ARGS__) \
TEST_ENCODE_DECODE_SHRINKING_INT64(NAME, VALUE, __VA_ARGS__)

#define TEST_ENCODE_DECODE_SHRINKING_INT16(NAME, VALUE, ...) \
TEST_ENCODE_DECODE_SHRINKING(Int16,NAME, i16(VALUE),  __VA_ARGS__) \
TEST_ENCODE_DECODE_SHRINKING_INT32(NAME, VALUE, __VA_ARGS__)

#define TEST_ENCODE_DECODE_SHRINKING_INT8(NAME, VALUE, ...) \
TEST_ENCODE_DECODE_SHRINKING(Int8, NAME, i8(VALUE),   __VA_ARGS__) \
TEST_ENCODE_DECODE_SHRINKING_INT16(NAME, VALUE, __VA_ARGS__)


#define TEST_ENCODE_DECODE_SHRINKING_FLOAT128(NAME, VALUE, ...) \
TEST_ENCODE_DECODE_SHRINKING(Float128, NAME, f128(VALUE),   __VA_ARGS__)

#define TEST_ENCODE_DECODE_SHRINKING_FLOAT64(NAME, VALUE, ...) \
TEST_ENCODE_DECODE_SHRINKING(Float64, NAME, f64(VALUE),   __VA_ARGS__) \
TEST_ENCODE_DECODE_SHRINKING_FLOAT128(NAME, VALUE, __VA_ARGS__)

#define TEST_ENCODE_DECODE_SHRINKING_FLOAT32(NAME, VALUE, ...) \
TEST_ENCODE_DECODE_SHRINKING(Float32, NAME, f32(VALUE),   __VA_ARGS__) \
TEST_ENCODE_DECODE_SHRINKING_FLOAT64(NAME, VALUE, __VA_ARGS__)


#define TEST_ENCODE_DECODE_SHRINKING_DECIMAL128(NAME, VALUE, ...) \
TEST_ENCODE_DECODE_SHRINKING(Decimal128, NAME, d128(VALUE),  __VA_ARGS__) \

#define TEST_ENCODE_DECODE_SHRINKING_DECIMAL64(NAME, VALUE, ...) \
TEST_ENCODE_DECODE_SHRINKING(Decimal64, NAME, d64(VALUE),  __VA_ARGS__) \
TEST_ENCODE_DECODE_SHRINKING_DECIMAL128(NAME, VALUE, __VA_ARGS__)

#define TEST_ENCODE_DECODE_SHRINKING_DECIMAL32(NAME, VALUE, ...) \
TEST_ENCODE_DECODE_SHRINKING(Decimal32, NAME, d32(VALUE),  __VA_ARGS__) \
TEST_ENCODE_DECODE_SHRINKING_DECIMAL64(NAME, VALUE, __VA_ARGS__)


// ==================================================================

TEST_ENCODE_DECODE_SHRINKING(Bool, false, bl(false), {0x7a})
TEST_ENCODE_DECODE_SHRINKING(Bool, true,  bl(true), {0x79})

TEST_ENCODE_DECODE_SHRINKING_INT8(_0,                                       0, {0x00})
TEST_ENCODE_DECODE_SHRINKING_INT8(_1,                                       1, {0x01})
TEST_ENCODE_DECODE_SHRINKING_INT8(_109,                                   109, {0x6d})
TEST_ENCODE_DECODE_SHRINKING_INT8(_n1,                                     -1, {0xff})
TEST_ENCODE_DECODE_SHRINKING_INT8(_n110,                                 -109, {0x93})
TEST_ENCODE_DECODE_SHRINKING_INT8(_110,                                   110, {0x6e, 0x6e, 0x00})
TEST_ENCODE_DECODE_SHRINKING_INT8(_n111,                                 -110, {0x6e, 0x92, 0xff})
TEST_ENCODE_DECODE_SHRINKING_INT8(_7f,                                   0x7f, {0x6e, 0x7f, 0x00})
TEST_ENCODE_DECODE_SHRINKING_INT8(_n80,                                 -0x80, {0x6e, 0x80, 0xff})
TEST_ENCODE_DECODE_SHRINKING_INT16(_7fff,                              0x7fff, {0x6e, 0xff, 0x7f})
TEST_ENCODE_DECODE_SHRINKING_INT16(_n8000,                            -0x8000, {0x6e, 0x00, 0x80})
TEST_ENCODE_DECODE_SHRINKING_INT32(_8000,                              0x8000, {0x6f, 0x00, 0x80, 0x00, 0x00})
TEST_ENCODE_DECODE_SHRINKING_INT32(_n8001,                            -0x8001, {0x6f, 0xff, 0x7f, 0xff, 0xff})
TEST_ENCODE_DECODE_SHRINKING_INT32(_7fffffff,                      0x7fffffff, {0x6f, 0xff, 0xff, 0xff, 0x7f})
TEST_ENCODE_DECODE_SHRINKING_INT32(_n80000000,                  -0x7fffffff-1, {0x6f, 0x00, 0x00, 0x00, 0x80})
TEST_ENCODE_DECODE_SHRINKING_INT64(_80000000,                     0x80000000L, {0x70, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00})
TEST_ENCODE_DECODE_SHRINKING_INT64(_n80000001,                   -0x80000001L, {0x70, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff})
TEST_ENCODE_DECODE_SHRINKING_INT64(_7fffffffffffffff,     0x7fffffffffffffffL, {0x70, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f})
TEST_ENCODE_DECODE_SHRINKING_INT64(_n8000000000000000, -0x7fffffffffffffffL-1, {0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80})

TEST_ENCODE_DECODE_SHRINKING_INT128(_10000000000000000,                  0x0000000000000001L, 0x0000000000000000L, {0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00})
TEST_ENCODE_DECODE_SHRINKING_INT128(_n10000000000000000,                -0x0000000000000001L, 0x0000000000000000L, {0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff})
TEST_ENCODE_DECODE_SHRINKING_INT128(_7fffffffffffffffffffffffffffffff,   0x7fffffffffffffffL, 0xffffffffffffffffL, {0x71, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f})
TEST_ENCODE_DECODE_SHRINKING_INT128(_n80000000000000000000000000000000, -0x8000000000000000L, 0x0000000000000000L, {0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80})

TEST_ENCODE_DECODE_SHRINKING_FLOAT32(_0_0,                0.0, {0x72, 0x00, 0x00, 0x00, 0x00})
TEST_ENCODE_DECODE_SHRINKING_FLOAT32(n967234_125, -967234.125, {0x72, 0x22, 0x24, 0x6c, 0xc9})
TEST_ENCODE_DECODE_SHRINKING_FLOAT64(_1_0123,                1.0123, {0x73, 0x51, 0xda, 0x1b, 0x7c, 0x61, 0x32, 0xf0, 0x3f})
TEST_ENCODE_DECODE_SHRINKING_FLOAT128(_1_01234,             1.01234l, {0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xba, 0x7e, 0xc1, 0x6e, 0xd8, 0xb6, 0x28, 0x03, 0xff, 0x3f})


TEST_ENCODE_DECODE_SHRINKING_DECIMAL32(_0,   0.0df, {0x75, 0x00, 0x00, 0x00, 0x32})
TEST_ENCODE_DECODE_SHRINKING_DECIMAL32(_0_5, 0.5df, {0x75, 0x05, 0x00, 0x00, 0x32})
TEST_ENCODE_DECODE_SHRINKING_DECIMAL32(_0_1, 0.1df, {0x75, 0x01, 0x00, 0x00, 0x32})
TEST_ENCODE_DECODE_SHRINKING(Decimal64, _1000000_000001, d64(1000000.000001dd), {0x76, 0x01, 0x10, 0xa5, 0xd4, 0xe8, 0x00, 0x00, 0x31})
TEST_ENCODE_DECODE_SHRINKING(Decimal128, _1000000000000_000000000001, d128(1000000000000.000000000001dl), {0x77, 0x01, 0x00, 0x00, 0xa1, 0xed, 0xcc, 0xce, 0x1b, 0xc2, 0xd3, 0x00, 0x00, 0x00, 0x00, 0x28, 0x30})

TEST_ENCODE_DECODE_SHRINKING(Empty, nil, nil(), {0x7e})

TEST_ENCODE(Padding, pad_1, 10, pad(1), {0x7f})
TEST_ENCODE(Padding, pad_2, 10, pad(2), {0x7f, 0x7f})

TEST_ENCODE_DECODE_SHRINKING(Time, zero, smtime(0), {0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00})
TEST_ENCODE_DECODE_SHRINKING_TIME(1955, 11,  5,  8, 21,  0,      0, {0x78, 0x00, 0x00, 0x00, 0x54, 0xa8, 0xe6, 0xe8, 0x01})
TEST_ENCODE_DECODE_SHRINKING_TIME(2015, 10, 21, 14, 28,  9, 714000, {0x78, 0x10, 0xe5, 0x9a, 0x70, 0xce, 0xe4, 0xf7, 0x01})
TEST_ENCODE_DECODE_SHRINKING_TIME(1985, 10, 26,  8, 22, 16, 900142, {0x78, 0x2e, 0xbc, 0x0d, 0x59, 0x68, 0x65, 0xf0, 0x01})

TEST_ENCODE_DECODE_SHRINKING_CONTAINER(String,  size_0, 1, str(make_string(0)),  {0x80})
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(String,  size_1, 1, str(make_string(1)),  concat({0x81}, as_vector(make_string(1))))
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(String,  size_2, 1, str(make_string(2)),  concat({0x82}, as_vector(make_string(2))))
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(String,  size_3, 1, str(make_string(3)),  concat({0x83}, as_vector(make_string(3))))
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(String,  size_4, 1, str(make_string(4)),  concat({0x84}, as_vector(make_string(4))))
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(String,  size_5, 1, str(make_string(5)),  concat({0x85}, as_vector(make_string(5))))
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(String,  size_6, 1, str(make_string(6)),  concat({0x86}, as_vector(make_string(6))))
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(String,  size_7, 1, str(make_string(7)),  concat({0x87}, as_vector(make_string(7))))
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(String,  size_8, 1, str(make_string(8)),  concat({0x88}, as_vector(make_string(8))))
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(String,  size_9, 1, str(make_string(9)),  concat({0x89}, as_vector(make_string(9))))
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(String, size_10, 1, str(make_string(10)), concat({0x8a}, as_vector(make_string(10))))
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(String, size_11, 1, str(make_string(11)), concat({0x8b}, as_vector(make_string(11))))
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(String, size_12, 1, str(make_string(12)), concat({0x8c}, as_vector(make_string(12))))
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(String, size_13, 1, str(make_string(13)), concat({0x8d}, as_vector(make_string(13))))
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(String, size_14, 1, str(make_string(14)), concat({0x8e}, as_vector(make_string(14))))
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(String, size_15, 1, str(make_string(15)), concat({0x8f}, as_vector(make_string(15))))
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(String, size_16, 2, str(make_string(16)), concat({0x90}, array_length_field(16), as_vector(make_string(16))))
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(String, size_500, 3, str(make_string(500)), concat({0x90}, array_length_field(500), as_vector(make_string(500))))
TEST_ENCODE_STATUS(String, too_long, CBE_ENCODE_ERROR_FIELD_LENGTH_EXCEEDED, strh(1)->data({0x30, 0x30}));
TEST_ENCODE_STATUS(String, too_short, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, strh(5)->data({0x30, 0x30}));
TEST_ENCODE_STATUS(String, int8, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, strh(5)->data({0x30})->i8(0));
TEST_ENCODE_STATUS(String, int16, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, strh(5)->data({0x30})->i16(0x7ff));
TEST_ENCODE_STATUS(String, int32, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, strh(5)->data({0x30})->i32(0x7ffff));
TEST_ENCODE_STATUS(String, int64, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, strh(5)->data({0x30})->i64(0x7ffffffffl));
TEST_ENCODE_STATUS(String, int128, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, strh(5)->data({0x30})->i128(0x7ffffffffl, 0));
TEST_ENCODE_STATUS(String, float32, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, strh(5)->data({0x30})->f32(0.1));
TEST_ENCODE_STATUS(String, float64, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, strh(5)->data({0x30})->f64(0.1));
TEST_ENCODE_STATUS(String, float128, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, strh(5)->data({0x30})->f128(0.1));
TEST_ENCODE_STATUS(String, decimal32, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, strh(5)->data({0x30})->d32(0.1));
TEST_ENCODE_STATUS(String, decimal64, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, strh(5)->data({0x30})->d64(0.1));
TEST_ENCODE_STATUS(String, decimal128, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, strh(5)->data({0x30})->d128(0.1));
TEST_ENCODE_STATUS(String, time, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, strh(5)->data({0x30})->smtime(0));
TEST_ENCODE_STATUS(String, boolean, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, strh(5)->data({0x30})->bl(false));
TEST_ENCODE_STATUS(String, list, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, strh(5)->data({0x30})->list()->end());
TEST_ENCODE_STATUS(String, map, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, strh(5)->data({0x30})->map()->end());
TEST_ENCODE_STATUS(String, end, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, strh(5)->data({0x30})->end());
TEST_ENCODE_STATUS(String, nil, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, strh(5)->data({0x30})->nil());
TEST_ENCODE_STATUS(String, padding, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, strh(5)->data({0x30})->pad(1));
TEST_ENCODE_STATUS(String, string, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, strh(5)->data({0x30})->str("a"));
TEST_ENCODE_STATUS(String, string16, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, strh(5)->data({0x30})->str("1234567890123456"));
TEST_ENCODE_STATUS(String, binary, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, strh(5)->data({0x30})->bin({0x00}));
TEST_DECODE_STATUS(StringDecode, too_short, CBE_DECODE_ERROR_INCOMPLETE_FIELD, {0x85, 0x30, 0x30});
TEST_DECODE_STATUS(StringDecode, too_short2, CBE_DECODE_ERROR_INCOMPLETE_FIELD, {0x90, 0x40, 0x30});

TEST_ENCODE_DECODE_SHRINKING_CONTAINER(Binary,  size_0, 2, bin(make_bytes(0)),  concat({0x91}, array_length_field(0)))
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(Binary,  size_1, 2, bin(make_bytes(1)),  concat({0x91}, array_length_field(1), make_bytes(1)))
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(Binary,  size_2, 2, bin(make_bytes(2)),  concat({0x91}, array_length_field(2), make_bytes(2)))
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(Binary,  size_500, 3, bin(make_bytes(500)),  concat({0x91}, array_length_field(500), make_bytes(500)))
TEST_ENCODE_STATUS(Binary, too_long, CBE_ENCODE_ERROR_FIELD_LENGTH_EXCEEDED, binh(1)->data({0x30, 0x30}));
TEST_ENCODE_STATUS(Binary, too_short, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30, 0x30}));
TEST_ENCODE_STATUS(Binary, int8, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->i8(0));
TEST_ENCODE_STATUS(Binary, int16, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->i16(0x7ff));
TEST_ENCODE_STATUS(Binary, int32, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->i32(0x7ffff));
TEST_ENCODE_STATUS(Binary, int64, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->i64(0x7ffffffffl));
TEST_ENCODE_STATUS(Binary, int128, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->i128(0x7ffffffffl, 0));
TEST_ENCODE_STATUS(Binary, float32, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->f32(0.1));
TEST_ENCODE_STATUS(Binary, float64, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->f64(0.1));
TEST_ENCODE_STATUS(Binary, float128, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->f128(0.1));
TEST_ENCODE_STATUS(Binary, decimal32, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->d32(0.1));
TEST_ENCODE_STATUS(Binary, decimal64, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->d64(0.1));
TEST_ENCODE_STATUS(Binary, decimal128, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->d128(0.1));
TEST_ENCODE_STATUS(Binary, time, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->smtime(0));
TEST_ENCODE_STATUS(Binary, boolean, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->bl(false));
TEST_ENCODE_STATUS(Binary, list, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->list()->end());
TEST_ENCODE_STATUS(Binary, map, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->map()->end());
TEST_ENCODE_STATUS(Binary, end, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->end());
TEST_ENCODE_STATUS(Binary, nil, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->nil());
TEST_ENCODE_STATUS(Binary, padding, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->pad(1));
TEST_ENCODE_STATUS(Binary, string, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->str("a"));
TEST_ENCODE_STATUS(Binary, string16, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->str("1234567890123456"));
TEST_ENCODE_STATUS(Binary, binary, CBE_ENCODE_ERROR_INCOMPLETE_FIELD, binh(5)->data({0x30})->bin({0x00}));
TEST_DECODE_STATUS(BinaryDecode, too_short, CBE_DECODE_ERROR_INCOMPLETE_FIELD, {0x91, 0x40, 0x30});

TEST_ENCODE_DECODE_SHRINKING_CONTAINER(Comment,  size_0, 2, comment(make_string(0)),  concat({0x92}, array_length_field(0)))
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(Comment, size_16, 2, comment(make_string(16)), concat({0x92}, array_length_field(16), as_vector(make_string(16))))

TEST_ENCODE_DECODE_SHRINKING_CONTAINER(List, size_0, 1, list()->end(), {0x7b, 0x7d})
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(List, size_1, 1, list()->i8(1)->end(), {0x7b, 0x01, 0x7d})
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(List, size_2, 1, list()->str("1")->i8(1)->end(), {0x7b, 0x81, 0x31, 0x01, 0x7d})
TEST_ENCODE_DECODE_STATUS(List, unterminated, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, CBE_DECODE_ERROR_UNBALANCED_CONTAINERS, list())
TEST_ENCODE_DECODE_STATUS(List, unterminated_2, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, CBE_DECODE_ERROR_UNBALANCED_CONTAINERS, list()->f32(0.1))
TEST_ENCODE_DECODE_STATUS(List, unterminated_3, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, CBE_DECODE_ERROR_UNBALANCED_CONTAINERS, list()->map())
// Can't test decode because ending the container ends the document.
TEST_ENCODE_STATUS(List, encode_extra_end, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, list()->end()->end())
TEST_ENCODE_STATUS(List, encode_extra_end_2, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, list()->map()->end()->end()->end())
TEST(List, too_deep)
{
	const int depth_too_far = 10001;
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;

    char cbe_encode_process_data[cbe_encode_process_size(MAX_CONTAINER_DEPTH)];
    struct cbe_encode_process* encode_process = (struct cbe_encode_process*)&cbe_encode_process_data ;
    cbe_encode_begin(encode_process, memory.data(), memory.size(), MAX_CONTAINER_DEPTH);
	cbe_encode_status status;
	for(int i = 0; i < depth_too_far; i++)
	{
		status = cbe_encode_list_begin(encode_process);
		if(status == CBE_ENCODE_ERROR_MAX_CONTAINER_DEPTH_EXCEEDED)
		{
			break;
		}
	}
	EXPECT_EQ(CBE_ENCODE_ERROR_MAX_CONTAINER_DEPTH_EXCEEDED, status);
}

TEST_ENCODE_DECODE_SHRINKING_CONTAINER(Map, size_0, 1, map()->end(), {0x7c, 0x7d})
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(Map, size_1, 1, map()->str("1")->i8(1)->end(), {0x7c, 0x81, 0x31, 0x01, 0x7d})
TEST_ENCODE_DECODE_SHRINKING_CONTAINER(Map, size_2, 1, map()->str("1")->i8(1)->str("2")->i8(2)->end(), {0x7c, 0x81, 0x31, 0x01, 0x81, 0x32, 0x02, 0x7d})
TEST_ENCODE_DECODE_STATUS(Map, unterminated, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, CBE_DECODE_ERROR_UNBALANCED_CONTAINERS, map())
TEST_ENCODE_DECODE_STATUS(Map, unterminated_2, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, CBE_DECODE_ERROR_UNBALANCED_CONTAINERS, map()->f32(0.1)->i8(1))
TEST_ENCODE_DECODE_STATUS(Map, unterminated_3, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, CBE_DECODE_ERROR_UNBALANCED_CONTAINERS, map()->str("")->list())
TEST_ENCODE_DECODE_STATUS(Map, unterminated_4, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, CBE_DECODE_ERROR_UNBALANCED_CONTAINERS, map()->str("")->list()->end())
// Can't test decode because ending the container ends the document.
TEST_ENCODE_STATUS(Map, encode_extra_end, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, map()->end()->end())
TEST_ENCODE_STATUS(Map, encode_extra_end_2, CBE_ENCODE_ERROR_UNBALANCED_CONTAINERS, map()->str("")->list()->end()->end()->end())

TEST_ENCODE_STATUS(Map, encode_nil_key, CBE_ENCODE_ERROR_INCORRECT_KEY_TYPE, map()->nil()->str("")->end())
TEST_DECODE_STATUS(Map, decode_nil_key, CBE_DECODE_ERROR_INCORRECT_KEY_TYPE, {0x7c, 0x7e, 0x80, 0x7d})
TEST_ENCODE_STATUS(Map, encode_list_key, CBE_ENCODE_ERROR_INCORRECT_KEY_TYPE, map()->list()->end()->str("")->end())
TEST_DECODE_STATUS(Map, decode_list_key, CBE_DECODE_ERROR_INCORRECT_KEY_TYPE, {0x7c, 0x7b, 0x7d, 0x80, 0x7d})
TEST_ENCODE_STATUS(Map, encode_map_key, CBE_ENCODE_ERROR_INCORRECT_KEY_TYPE, map()->map()->end()->str("")->end())
TEST_DECODE_STATUS(Map, decode_map_key, CBE_DECODE_ERROR_INCORRECT_KEY_TYPE, {0x7c, 0x7c, 0x7d, 0x80, 0x7d})

TEST_ENCODE_STATUS(Map, encode_missing_value_i8, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->i8(0)->end())
TEST_DECODE_STATUS(Map, decode_missing_value_i8, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x00, 0x7d})
TEST_ENCODE_STATUS(Map, encode_missing_value_i16, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->i16(0xfff)->end())
TEST_DECODE_STATUS(Map, decode_missing_value_i16, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x6e, 0xff, 0x0f, 0x7d})
TEST_ENCODE_STATUS(Map, encode_missing_value_i32, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->i32(0xfffff)->end())
TEST_DECODE_STATUS(Map, decode_missing_value_i32, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x6f, 0xff, 0xff, 0xff, 0x0f, 0x7d})
TEST_ENCODE_STATUS(Map, encode_missing_value_i64, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->i64(0xfffffffff)->end())
TEST_DECODE_STATUS(Map, decode_missing_value_i64, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x70, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x7d})
TEST_ENCODE_STATUS(Map, encode_missing_value_i128, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->i128(0x0f, 0xffffffffffffffff)->end())
TEST_DECODE_STATUS(Map, decode_missing_value_i128, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x71, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7d})
TEST_ENCODE_STATUS(Map, encode_missing_value_f32, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->f32(0.1)->end())
TEST_DECODE_STATUS(Map, decode_missing_value_f32, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x72, 0xcd, 0xcc, 0xcc, 0x3d, 0x7d})
TEST_ENCODE_STATUS(Map, encode_missing_value_f64, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->f64(1.0123)->end())
TEST_DECODE_STATUS(Map, decode_missing_value_f64, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x73, 0x51, 0xda, 0x1b, 0x7c, 0x61, 0x32, 0xf0, 0x3f, 0x7d})
TEST_ENCODE_STATUS(Map, encode_missing_value_f128, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->f128(1.012345l)->end())
TEST_DECODE_STATUS(Map, decode_missing_value_f128, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5e, 0x0c, 0xe5, 0x44, 0xbb, 0x0a, 0x29, 0x03, 0xff, 0x3f, 0x7d})
TEST_ENCODE_STATUS(Map, encode_missing_value_d32, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->d32(0.1)->end())
TEST_DECODE_STATUS(Map, decode_missing_value_d32, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x75, 0x01, 0x00, 0x00, 0x32, 0x7d})
TEST_ENCODE_STATUS(Map, encode_missing_value_d64, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->d64(1000000.000001dd)->end())
TEST_DECODE_STATUS(Map, decode_missing_value_d64, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x76, 0x01, 0x10, 0xa5, 0xd4, 0xe8, 0x00, 0x00, 0x31, 0x7d})
TEST_ENCODE_STATUS(Map, encode_missing_value_d128, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->d128(1000000000000.000000000001dl)->end())
TEST_DECODE_STATUS(Map, decode_missing_value_d128, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x77, 0x01, 0x00, 0x00, 0xa1, 0xed, 0xcc, 0xce, 0x1b, 0xc2, 0xd3, 0x00, 0x00, 0x00, 0x00, 0x28, 0x30, 0x7d})
TEST_ENCODE_STATUS(Map, encode_missing_value_time, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->smtime(0)->end())
TEST_DECODE_STATUS(Map, decode_missing_value_time, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7d})
TEST_ENCODE_STATUS(Map, encode_missing_value_string, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->str("")->end())
TEST_DECODE_STATUS(Map, decode_missing_value_string, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x80, 0x7d})
TEST_ENCODE_STATUS(Map, encode_missing_value_long_string, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->str("0000000000000000")->end())
TEST_DECODE_STATUS(Map, decode_missing_value_long_string, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x90, 0x40, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x7d})
TEST_ENCODE_STATUS(Map, encode_missing_value_map, CBE_ENCODE_ERROR_MISSING_VALUE_FOR_KEY, map()->str("")->map()->str("")->end()->end())
TEST_DECODE_STATUS(Map, decode_missing_value_map, CBE_DECODE_ERROR_MISSING_VALUE_FOR_KEY, {0x7c, 0x80, 0x7c, 0x80, 0x7d, 0x7d})

TEST_STOP_IN_CALLBACK(SIC, nil, nil())
TEST_STOP_IN_CALLBACK(SIC, bool, bl(false))
TEST_STOP_IN_CALLBACK(SIC, int8, i8(1))
TEST_STOP_IN_CALLBACK(SIC, int16, i16(1))
TEST_STOP_IN_CALLBACK(SIC, int32, i32(1))
TEST_STOP_IN_CALLBACK(SIC, int64, i64(1))
TEST_STOP_IN_CALLBACK(SIC, int128, i128(0, 1))
TEST_STOP_IN_CALLBACK(SIC, float32, f32(1))
TEST_STOP_IN_CALLBACK(SIC, float64, f64(1))
TEST_STOP_IN_CALLBACK(SIC, float128, f128(1))
TEST_STOP_IN_CALLBACK(SIC, decimal32, d32(1))
TEST_STOP_IN_CALLBACK(SIC, decimal64, d64(1))
TEST_STOP_IN_CALLBACK(SIC, decimal128, d128(1))
TEST_STOP_IN_CALLBACK(SIC, smalltime, smtime(1))
TEST_STOP_IN_CALLBACK(SIC, list, list()->end())
TEST_STOP_IN_CALLBACK(SIC, map, map()->end())
TEST_STOP_IN_CALLBACK(SIC, string, str("test"))
TEST_STOP_IN_CALLBACK(SIC, binary, bin(std::vector<uint8_t>()))
