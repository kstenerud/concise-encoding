#include "old_test_helpers.h"

static inline __int128 ii128(int64_t high, uint64_t low)
{
    return (((__int128)high) << 64) + low;
}

DEFINE_ENCODE_DECODE_ENCODE_TEST(Int64Test, _n80000000, (int64_t)-0x80000000L, {0x6f, 0x00, 0x00, 0x00, 0x80})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int128Test, _n80000000, (__int128)-0x80000000L, {0x6f, 0x00, 0x00, 0x00, 0x80})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int128Test, _n8000000000000000, (__int128)(int64_t)-0x8000000000000000L, {0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int128Test, _7fffffffffffffffffffffffffffffff, ii128(0x7fffffffffffffffL, 0xffffffffffffffffL), {0x71, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f})

// TODO: Separate incomplete tests for everything
DEFINE_ADD_INCOMPLETE_TEST(Int64Test, add_incomplete, 7, 0x7fffffffffffffffL)
DEFINE_ADD_INCOMPLETE_TEST(Int128Test, add_incomplete, 15, ii128(0x7fffffffffffffffL, 0xffffffffffffffffL))
DEFINE_ADD_INCOMPLETE_TEST(BoolTest, add_incomplete_true, 0, true)
DEFINE_ADD_INCOMPLETE_TEST(BoolTest, add_incomplete_false, 0, false)
DEFINE_ADD_INCOMPLETE_TEST(Float128Test, add_incomplete, 15, (__float128)1 / 10)
DEFINE_ADD_INCOMPLETE_TEST(Float32Test, add_incomplete, 4, 0.1f)
