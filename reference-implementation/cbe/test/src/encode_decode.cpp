#include "test_helpers.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"

using namespace enc;

#define DEFINE_TIME_TEST(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, USEC, ...) \
TEST(Time, time_ ## YEAR ## _ ## MONTH ## _ ## DAY ## _ ## HOUR ## _ ## MINUTE ## _ ## SECOND ## _ ## USEC) \
{ \
	cbe_test::expect_encode_decode_equality(0, \
		smtime(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, USEC), \
		 __VA_ARGS__); \
}


#define DEFINE_INT128_TEST_SUITE(NAME, HIGH, LOW, ...) \
TEST_ENCODE_DECODE(Int128, NAME, i128(HIGH, LOW),  __VA_ARGS__) \

#define DEFINE_INT128_TEST_SUITE_SIMPLE(NAME, VALUE, ...) \
TEST_ENCODE_DECODE(Int128, NAME, i128(VALUE),  __VA_ARGS__) \

#define DEFINE_INT64_TEST_SUITE(NAME, VALUE, ...) \
TEST_ENCODE_DECODE(Int64, NAME, i64(VALUE),  __VA_ARGS__) \
DEFINE_INT128_TEST_SUITE_SIMPLE(NAME, VALUE, __VA_ARGS__)

#define DEFINE_INT32_TEST_SUITE(NAME, VALUE, ...) \
TEST_ENCODE_DECODE(Int32,NAME, i32(VALUE),  __VA_ARGS__) \
DEFINE_INT64_TEST_SUITE(NAME, VALUE, __VA_ARGS__)

#define DEFINE_INT16_TEST_SUITE(NAME, VALUE, ...) \
TEST_ENCODE_DECODE(Int16,NAME, i16(VALUE),  __VA_ARGS__) \
DEFINE_INT32_TEST_SUITE(NAME, VALUE, __VA_ARGS__)

#define DEFINE_INT8_TEST_SUITE(NAME, VALUE, ...) \
TEST_ENCODE_DECODE(Int8, NAME, i8(VALUE),   __VA_ARGS__) \
DEFINE_INT16_TEST_SUITE(NAME, VALUE, __VA_ARGS__)


#define DEFINE_FLOAT128_TEST_SUITE(NAME, VALUE, ...) \
TEST_ENCODE_DECODE(Float128, NAME, f128(VALUE),   __VA_ARGS__)

#define DEFINE_FLOAT64_TEST_SUITE(NAME, VALUE, ...) \
TEST_ENCODE_DECODE(Float64, NAME, f64(VALUE),   __VA_ARGS__) \
DEFINE_FLOAT128_TEST_SUITE(NAME, VALUE, __VA_ARGS__)

#define DEFINE_FLOAT32_TEST_SUITE(NAME, VALUE, ...) \
TEST_ENCODE_DECODE(Float32, NAME, f32(VALUE),   __VA_ARGS__) \
DEFINE_FLOAT64_TEST_SUITE(NAME, VALUE, __VA_ARGS__)


#define DEFINE_DECIMAL128_TEST_SUITE(NAME, VALUE, ...) \
TEST_ENCODE_DECODE(Decimal128, NAME, d128(VALUE),  __VA_ARGS__) \

#define DEFINE_DECIMAL64_TEST_SUITE(NAME, VALUE, ...) \
TEST_ENCODE_DECODE(Decimal64, NAME, d64(VALUE),  __VA_ARGS__) \
DEFINE_DECIMAL128_TEST_SUITE(NAME, VALUE, __VA_ARGS__)

#define DEFINE_DECIMAL32_TEST_SUITE(NAME, VALUE, ...) \
TEST_ENCODE_DECODE(Decimal32, NAME, d32(VALUE),  __VA_ARGS__) \
DEFINE_DECIMAL64_TEST_SUITE(NAME, VALUE, __VA_ARGS__)


// ==================================================================

TEST_ENCODE_DECODE(Bool, false, bl(false), {0x7a})
TEST_ENCODE_DECODE(Bool, true,  bl(true), {0x79})

DEFINE_INT8_TEST_SUITE(_0,                                       0, {0x00})
DEFINE_INT8_TEST_SUITE(_1,                                       1, {0x01})
DEFINE_INT8_TEST_SUITE(_109,                                   109, {0x6d})
DEFINE_INT8_TEST_SUITE(_n1,                                     -1, {0xff})
DEFINE_INT8_TEST_SUITE(_n110,                                 -110, {0x92})
DEFINE_INT8_TEST_SUITE(_110,                                   110, {0x6e, 0x6e, 0x00})
DEFINE_INT8_TEST_SUITE(_n111,                                 -111, {0x6e, 0x91, 0xff})
DEFINE_INT8_TEST_SUITE(_7f,                                   0x7f, {0x6e, 0x7f, 0x00})
DEFINE_INT8_TEST_SUITE(_n80,                                 -0x80, {0x6e, 0x80, 0xff})
DEFINE_INT16_TEST_SUITE(_7fff,                              0x7fff, {0x6e, 0xff, 0x7f})
DEFINE_INT16_TEST_SUITE(_n8000,                            -0x8000, {0x6e, 0x00, 0x80})
DEFINE_INT32_TEST_SUITE(_8000,                              0x8000, {0x6f, 0x00, 0x80, 0x00, 0x00})
DEFINE_INT32_TEST_SUITE(_n8001,                            -0x8001, {0x6f, 0xff, 0x7f, 0xff, 0xff})
DEFINE_INT32_TEST_SUITE(_7fffffff,                      0x7fffffff, {0x6f, 0xff, 0xff, 0xff, 0x7f})
DEFINE_INT32_TEST_SUITE(_n80000000,                  -0x7fffffff-1, {0x6f, 0x00, 0x00, 0x00, 0x80})
DEFINE_INT64_TEST_SUITE(_80000000,                     0x80000000L, {0x70, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00})
DEFINE_INT64_TEST_SUITE(_n80000001,                   -0x80000001L, {0x70, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff})
DEFINE_INT64_TEST_SUITE(_7fffffffffffffff,     0x7fffffffffffffffL, {0x70, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f})
DEFINE_INT64_TEST_SUITE(_n8000000000000000, -0x7fffffffffffffffL-1, {0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80})

DEFINE_INT128_TEST_SUITE(_10000000000000000,                  0x0000000000000001L, 0x0000000000000000L, {0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00})
DEFINE_INT128_TEST_SUITE(_n10000000000000000,                -0x0000000000000001L, 0x0000000000000000L, {0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff})
DEFINE_INT128_TEST_SUITE(_7fffffffffffffffffffffffffffffff,   0x7fffffffffffffffL, 0xffffffffffffffffL, {0x71, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f})
DEFINE_INT128_TEST_SUITE(_n80000000000000000000000000000000, -0x8000000000000000L, 0x0000000000000000L, {0x71, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80})

DEFINE_FLOAT32_TEST_SUITE(_0_0,                0.0, {0x72, 0x00, 0x00, 0x00, 0x00})
DEFINE_FLOAT32_TEST_SUITE(_0_1,                0.1, {0x72, 0xcd, 0xcc, 0xcc, 0x3d})
DEFINE_FLOAT32_TEST_SUITE(_0_000014,      0.000014, {0x72, 0x8b, 0xe1, 0x6a, 0x37})
DEFINE_FLOAT32_TEST_SUITE(n967234_125, -967234.125, {0x72, 0x22, 0x24, 0x6c, 0xc9})
// TODO: float64 and float128 numbers


DEFINE_DECIMAL32_TEST_SUITE(_0,   0.0df, {0x75, 0x00, 0x00, 0x00, 0x32})
DEFINE_DECIMAL32_TEST_SUITE(_0_5, 0.5df, {0x75, 0x05, 0x00, 0x00, 0x32})
DEFINE_DECIMAL32_TEST_SUITE(_0_1, 0.1df, {0x75, 0x01, 0x00, 0x00, 0x32})

TEST_ENCODE_DECODE(Decimal64, _1000000_000001, d64(1000000.000001dd), {0x76, 0x01, 0x10, 0xa5, 0xd4, 0xe8, 0x00, 0x00, 0x31})
// TODO: Produces a decimal32
//TEST_ENCODE_DECODE(Decimal128, _1000000000000_000000000001, d128(1000000000000.000000000001dl), {0x77, 0x01, 0x00, 0x00, 0xa1, 0xed, 0xcc, 0xce, 0x1b, 0xc2, 0xd3, 0x00, 0x00, 0x00, 0x00, 0x28, 0x30})

TEST_ENCODE_DECODE(Empty, empty, empty(), {0x7e})

TEST_ENCODE(Padding, pad_1, 10, pad(1), {0x7f})
TEST_ENCODE(Padding, pad_2, 10, pad(2), {0x7f, 0x7f})

TEST_ENCODE_DECODE(Time, zero, smtime(0), {0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00})
DEFINE_TIME_TEST(1955, 11,  5,  8, 21,  0,      0, {0x78, 0x00, 0x00, 0x00, 0x54, 0xa8, 0xe6, 0xe8, 0x01})
DEFINE_TIME_TEST(2015, 10, 21, 14, 28,  9, 714000, {0x78, 0x10, 0xe5, 0x9a, 0x70, 0xce, 0xe4, 0xf7, 0x01})
DEFINE_TIME_TEST(1985, 10, 26,  8, 22, 16, 900142, {0x78, 0x2e, 0xbc, 0x0d, 0x59, 0x68, 0x65, 0xf0, 0x01})

TEST_ENCODE_DECODE(String,  size_0, str(""), {0x80})
TEST_ENCODE_DECODE_CONTAINER(String,  size_1, 1, str("1"), {0x81, 0x31})
TEST_ENCODE_DECODE_CONTAINER(String,  size_2, 1, str("12"), {0x82, 0x31, 0x32})
TEST_ENCODE_DECODE_CONTAINER(String,  size_3, 1, str("123"), {0x83, 0x31, 0x32, 0x33})
TEST_ENCODE_DECODE_CONTAINER(String,  size_4, 1, str("1234"), {0x84, 0x31, 0x32, 0x33, 0x34})
TEST_ENCODE_DECODE_CONTAINER(String,  size_5, 1, str("12345"), {0x85, 0x31, 0x32, 0x33, 0x34, 0x35})
TEST_ENCODE_DECODE_CONTAINER(String,  size_6, 1, str("123456"), {0x86, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36})
TEST_ENCODE_DECODE_CONTAINER(String,  size_7, 1, str("1234567"), {0x87, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37})
TEST_ENCODE_DECODE_CONTAINER(String,  size_8, 1, str("12345678"), {0x88, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38})
TEST_ENCODE_DECODE_CONTAINER(String,  size_9, 1, str("123456789"), {0x89, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39})
TEST_ENCODE_DECODE_CONTAINER(String, size_10, 1, str("1234567890"), {0x8a, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30})
TEST_ENCODE_DECODE_CONTAINER(String, size_11, 1, str("12345678901"), {0x8b, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31})
TEST_ENCODE_DECODE_CONTAINER(String, size_12, 1, str("123456789012"), {0x8c, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32})
TEST_ENCODE_DECODE_CONTAINER(String, size_13, 1, str("1234567890123"), {0x8d, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33})
TEST_ENCODE_DECODE_CONTAINER(String, size_14, 1, str("12345678901234"), {0x8e, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34})
TEST_ENCODE_DECODE_CONTAINER(String, size_15, 1, str("123456789012345"), {0x8f, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35})
TEST_ENCODE_DECODE_CONTAINER(String, size_16, 2, str("1234567890123456"), {0x90, 0x40, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36})

TEST_ENCODE_DECODE(List, empty, list()->end(), {0x7b, 0x7d})
TEST_ENCODE_DECODE(List, one, list()->i8(1)->end(), {0x7b, 0x01, 0x7d})
