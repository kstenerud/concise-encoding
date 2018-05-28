#include "test_helpers.h"

static inline __int128 i128(int64_t high, uint64_t low)
{
	return (((__int128)high) << 64) + low;
}

DEFINE_SCALAR_WRITE_TEST(Int128Test, add_0,         (__int128)          0L, {0x00})
DEFINE_SCALAR_WRITE_TEST(Int128Test, add_1,         (__int128)          1L, {0x01})
DEFINE_SCALAR_WRITE_TEST(Int128Test, add_100,       (__int128)        100L, {0x64})
DEFINE_SCALAR_WRITE_TEST(Int128Test, add_n1,        (__int128)         -1L, {0xff})
DEFINE_SCALAR_WRITE_TEST(Int128Test, add_n100,      (__int128)       -100L, {0x9c})
DEFINE_SCALAR_WRITE_TEST(Int128Test, add_101,       (__int128)        101L, {TYPE_INT_16, 0x65, 0x00})
DEFINE_SCALAR_WRITE_TEST(Int128Test, add_n101,      (__int128)       -101L, {TYPE_INT_16, 0x9b, 0xff})
DEFINE_SCALAR_WRITE_TEST(Int128Test, add_127,       (__int128)        127L, {TYPE_INT_16, 0x7f, 0x00})
DEFINE_SCALAR_WRITE_TEST(Int128Test, add_n128,      (__int128)       -128L, {TYPE_INT_16, 0x80, 0xff})
DEFINE_SCALAR_WRITE_TEST(Int128Test, add_7fff,      (__int128)     0x7fffL, {TYPE_INT_16, 0xff, 0x7f})
DEFINE_SCALAR_WRITE_TEST(Int128Test, add_n8000,     (__int128)    -0x8000L, {TYPE_INT_16, 0x00, 0x80})
DEFINE_SCALAR_WRITE_TEST(Int128Test, add_8000,      (__int128)     0x8000L, {TYPE_INT_32, 0x00, 0x80, 0x00, 0x00})
DEFINE_SCALAR_WRITE_TEST(Int128Test, add_n8001,     (__int128)    -0x8001L, {TYPE_INT_32, 0xff, 0x7f, 0xff, 0xff})
DEFINE_SCALAR_WRITE_TEST(Int128Test, add_7fffffff,  (__int128) 0x7fffffffL, {TYPE_INT_32, 0xff, 0xff, 0xff, 0x7f})
DEFINE_SCALAR_WRITE_TEST(Int128Test, add_n80000000, (__int128)-0x80000000L, {TYPE_INT_32, 0x00, 0x00, 0x00, 0x80})
DEFINE_SCALAR_WRITE_TEST(Int128Test, add_80000000,  (__int128) 0x80000000L, {TYPE_INT_64, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00})
DEFINE_SCALAR_WRITE_TEST(Int128Test, add_n80000001, (__int128)-0x80000001L, {TYPE_INT_64, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff})
DEFINE_SCALAR_WRITE_TEST(Int128Test, add_7fffffffffffffff,  (__int128) 0x7fffffffffffffffL, {TYPE_INT_64, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f})
DEFINE_SCALAR_WRITE_TEST(Int128Test, add_n8000000000000000, (__int128)(int64_t)-0x8000000000000000L, {TYPE_INT_64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80})
DEFINE_SCALAR_WRITE_TEST(Int128Test, add_10000000000000000,  i128(0x0000000000000001L, 0x0000000000000000L), {TYPE_INT_128, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00})
DEFINE_SCALAR_WRITE_TEST(Int128Test, add_n10000000000000000,  i128(-0x0000000000000001L, 0x0000000000000000L), {TYPE_INT_128, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff})
DEFINE_SCALAR_WRITE_TEST(Int128Test, add_7fffffffffffffffffffffffffffffff, i128(0x7fffffffffffffffL, 0xffffffffffffffffL), {TYPE_INT_128, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f})
DEFINE_SCALAR_WRITE_TEST(Int128Test, add_n80000000000000000000000000000000,  i128(-0x8000000000000000L, 0x0000000000000000L), {TYPE_INT_128, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80})
