#include "helpers/test_helpers.h"

// #define KSLogger_LocalLevel DEBUG
#include "kslogger.h"

using namespace enc;

#define TEST_ENCODE_DECODE_SHRINKING_TIME(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, USEC, ...) \
TEST(Time, time_ ## YEAR ## _ ## MONTH ## _ ## DAY ## _ ## HOUR ## _ ## MINUTE ## _ ## SECOND ## _ ## USEC) \
{ \
    cbe_test::expect_encode_decode_with_shrinking_buffer_size(0, \
        time(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, USEC), \
         __VA_ARGS__); \
}


#define TEST_ENCODE_DECODE_SHRINKING_NINT128(NAME, HIGH, LOW, ...) \
TEST_ENCODE_DECODE_SHRINKING(Int, NAME, 0, si(HIGH, LOW),  __VA_ARGS__) \

#define TEST_ENCODE_DECODE_SHRINKING_NINT(NAME, VALUE, ...) \
TEST_ENCODE_DECODE_SHRINKING(Int, NAME, 0, si(VALUE),  __VA_ARGS__) \

#define TEST_ENCODE_DECODE_SHRINKING_PINT128(NAME, HIGH, LOW, ...) \
TEST_ENCODE_DECODE_SHRINKING(Int, NAME, 0, ui(HIGH, LOW),  __VA_ARGS__) \

#define TEST_ENCODE_DECODE_SHRINKING_PINT(NAME, VALUE, ...) \
TEST_ENCODE_DECODE_SHRINKING(Int, NAME, 0, ui(VALUE),  __VA_ARGS__) \

#define TEST_ENCODE_DECODE_SHRINKING_FLOAT(NAME, VALUE, ...) \
TEST_ENCODE_DECODE_SHRINKING(Float, NAME, 0, flt(VALUE),   __VA_ARGS__)

#define TEST_ENCODE_DECODE_SHRINKING_DEC(NAME, VALUE, ...) \
TEST_ENCODE_DECODE_SHRINKING(Decimal, NAME, 0, dec(VALUE),  __VA_ARGS__) \


// ==================================================================

TEST_ENCODE_DECODE_SHRINKING(Bool, false, 0, bl(false), {0x7c})
TEST_ENCODE_DECODE_SHRINKING(Bool, true,  0, bl(true), {0x7d})

TEST_ENCODE_DECODE_SHRINKING_PINT(p0,                                       0, {0x00})
TEST_ENCODE_DECODE_SHRINKING_PINT(p1,                                       1, {0x01})
TEST_ENCODE_DECODE_SHRINKING_PINT(p109,                                   106, {0x6a})
TEST_ENCODE_DECODE_SHRINKING_NINT(n1,                                      -1, {0xff})
TEST_ENCODE_DECODE_SHRINKING_NINT(n110,                                  -106, {0x96})
TEST_ENCODE_DECODE_SHRINKING_PINT(p110,                                   107, {0x6b, 0x6b})
TEST_ENCODE_DECODE_SHRINKING_NINT(n111,                                  -107, {0x70, 0x6b})
TEST_ENCODE_DECODE_SHRINKING_PINT(px7f,                                  0x7f, {0x6b, 0x7f})
TEST_ENCODE_DECODE_SHRINKING_NINT(nx80,                                 -0x80, {0x70, 0x80})
TEST_ENCODE_DECODE_SHRINKING_PINT(pxff,                                  0xff, {0x6b, 0xff})
TEST_ENCODE_DECODE_SHRINKING_NINT(nxff,                                 -0xff, {0x70, 0xff})

TEST_ENCODE_DECODE_SHRINKING_PINT(px100,                               0x100, {0x6c, 0x00, 0x01})
TEST_ENCODE_DECODE_SHRINKING_NINT(nx100,                              -0x100, {0x71, 0x00, 0x01})
TEST_ENCODE_DECODE_SHRINKING_PINT(p7fff,                              0x7fff, {0x6c, 0xff, 0x7f})
TEST_ENCODE_DECODE_SHRINKING_NINT(n8000,                             -0x8000, {0x71, 0x00, 0x80})
TEST_ENCODE_DECODE_SHRINKING_PINT(pffff,                              0xffff, {0x6c, 0xff, 0xff})
TEST_ENCODE_DECODE_SHRINKING_NINT(nffff,                             -0xffff, {0x71, 0xff, 0xff})

TEST_ENCODE_DECODE_SHRINKING_PINT(p10000,                            0x10000, {0x6d, 0x00, 0x00, 0x01, 0x00})
TEST_ENCODE_DECODE_SHRINKING_NINT(n10000,                           -0x10000, {0x72, 0x00, 0x00, 0x01, 0x00})
TEST_ENCODE_DECODE_SHRINKING_PINT(p7fffffff,                      0x7fffffff, {0x6d, 0xff, 0xff, 0xff, 0x7f})
TEST_ENCODE_DECODE_SHRINKING_NINT(n80000000,                    -0x80000000L, {0x72, 0x00, 0x00, 0x00, 0x80})
TEST_ENCODE_DECODE_SHRINKING_PINT(pffffffff,                      0xffffffff, {0x6d, 0xff, 0xff, 0xff, 0xff})
TEST_ENCODE_DECODE_SHRINKING_NINT(nffffffff,                    -0xffffffffL, {0x72, 0xff, 0xff, 0xff, 0xff})

TEST_ENCODE_DECODE_SHRINKING_PINT(p100000000,                   0x100000000L, {0x6e, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00})
TEST_ENCODE_DECODE_SHRINKING_NINT(n100000000,                  -0x100000000L, {0x73, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00})
TEST_ENCODE_DECODE_SHRINKING_PINT(p7fffffffffffffff,     0x7fffffffffffffffL, {0x6e, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f})
TEST_ENCODE_DECODE_SHRINKING_NINT(n8000000000000000,  -0x7fffffffffffffffL-1, {0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80})
TEST_ENCODE_DECODE_SHRINKING_PINT(pffffffffffffffff,     0xffffffffffffffffL, {0x6e, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff})
// TODO: Fix negative values
// TEST_ENCODE_DECODE_SHRINKING_NINT128(nffffffffffffffff, -1, 0x0000000000000000L, {0x73, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff})

TEST_ENCODE_DECODE_SHRINKING_PINT128(p10000000000000000,                  0x0000000000000001L, 0x0000000000000000L, {0x6f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00})
// TEST_ENCODE_DECODE_SHRINKING_NINT128(n10000000000000000,                 -0x0000000000000001L, 0x0000000000000000L, {0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00})
TEST_ENCODE_DECODE_SHRINKING_PINT128(p7fffffffffffffffffffffffffffffff,   0x7fffffffffffffffL, 0xffffffffffffffffL, {0x6f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f})
// TEST_ENCODE_DECODE_SHRINKING_NINT128(n80000000000000000000000000000000,  -0x8000000000000000L, 0x0000000000000000L, {0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80})

TEST_ENCODE_DECODE_SHRINKING_FLOAT(p0_0,        0.0,         {0x75, 0x00, 0x00, 0x00, 0x00})
TEST_ENCODE_DECODE_SHRINKING_FLOAT(n967234_125, -967234.125, {0x75, 0x22, 0x24, 0x6c, 0xc9})
TEST_ENCODE_DECODE_SHRINKING_FLOAT(p1_0123,     1.0123,      {0x76, 0x51, 0xda, 0x1b, 0x7c, 0x61, 0x32, 0xf0, 0x3f})
TEST_ENCODE_DECODE_SHRINKING_FLOAT(p1_01234,   1.01234l,     {0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xba, 0x7e, 0xc1, 0x6e, 0xd8, 0xb6, 0x28, 0x03, 0xff, 0x3f})

TEST_ENCODE_DECODE_SHRINKING_DEC(p0,   0.0df, {0x78, 0x00, 0x00, 0x00, 0x32})
TEST_ENCODE_DECODE_SHRINKING_DEC(p0_5, 0.5df, {0x78, 0x05, 0x00, 0x00, 0x32})
TEST_ENCODE_DECODE_SHRINKING_DEC(p0_1, 0.1df, {0x78, 0x01, 0x00, 0x00, 0x32})
TEST_ENCODE_DECODE_SHRINKING_DEC(p1000000_000001, 1000000.000001dd, {0x79, 0x01, 0x10, 0xa5, 0xd4, 0xe8, 0x00, 0x00, 0x31})
TEST_ENCODE_DECODE_SHRINKING_DEC(p1000000000000_000000000001, 1000000000000.000000000001dl, {0x7a, 0x01, 0x00, 0x00, 0xa1, 0xed, 0xcc, 0xce, 0x1b, 0xc2, 0xd3, 0x00, 0x00, 0x00, 0x00, 0x28, 0x30})

TEST_ENCODE_DECODE_SHRINKING(Time, zero, 0, enc::time(0), {0x7b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00})
TEST_ENCODE_DECODE_SHRINKING_TIME(1955, 11,  5,  8, 21,  0,      0, {0x7b, 0x00, 0x00, 0x00, 0x54, 0xa8, 0xec, 0xe8, 0x01})
TEST_ENCODE_DECODE_SHRINKING_TIME(2015, 10, 21, 14, 28,  9, 714000, {0x7b, 0x10, 0xe5, 0x9a, 0x70, 0xae, 0xea, 0xf7, 0x01})
TEST_ENCODE_DECODE_SHRINKING_TIME(1985, 10, 26,  8, 22, 16, 900142, {0x7b, 0x2e, 0xbc, 0x0d, 0x59, 0x48, 0x6b, 0xf0, 0x01})

TEST_ENCODE_DECODE_SHRINKING(Empty, nil, 0, nil(), {0x7e})

TEST_ENCODE_DATA(Padding, pad_1, 99, 9, pad(1), {0x7f})
TEST_ENCODE_DATA(Padding, pad_2, 99, 9, pad(2), {0x7f, 0x7f})

TEST_STOP_IN_CALLBACK(SIC, Nil, nil())
TEST_STOP_IN_CALLBACK(SIC, Bool, bl(false))
TEST_STOP_IN_CALLBACK(SIC, Int, si(0, 1))
TEST_STOP_IN_CALLBACK(SIC, Float, flt(1))
TEST_STOP_IN_CALLBACK(SIC, Decimal, dec(1))
TEST_STOP_IN_CALLBACK(SIC, Time, time(1))
TEST_STOP_IN_CALLBACK(SIC, List, list()->end())
TEST_STOP_IN_CALLBACK(SIC, Map, map()->end())
TEST_STOP_IN_CALLBACK(SIC, String, str("test"))
TEST_STOP_IN_CALLBACK(SIC, Binary, bin(std::vector<uint8_t>()))
