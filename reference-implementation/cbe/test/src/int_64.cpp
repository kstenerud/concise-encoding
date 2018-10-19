#include "test_helpers.h"

DEFINE_ENCODE_DECODE_ENCODE_TEST(Int64Test, _0,         (int64_t)          0L, {0x00})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int64Test, _1,         (int64_t)          1L, {0x01})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int64Test, _103,       (int64_t)        103L, {0x67})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int64Test, _n1,        (int64_t)         -1L, {0xff})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int64Test, _n104,      (int64_t)       -104L, {0x98})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int64Test, _104,       (int64_t)        104L, {0x8e, 0x68, 0x00})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int64Test, _n105,      (int64_t)       -105L, {0x8e, 0x97, 0xff})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int64Test, _127,       (int64_t)        127L, {0x8e, 0x7f, 0x00})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int64Test, _n128,      (int64_t)       -128L, {0x8e, 0x80, 0xff})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int64Test, _7fff,      (int64_t)     0x7fffL, {0x8e, 0xff, 0x7f})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int64Test, _n8000,     (int64_t)    -0x8000L, {0x8e, 0x00, 0x80})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int64Test, _8000,      (int64_t)     0x8000L, {0x8f, 0x00, 0x80, 0x00, 0x00})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int64Test, _n8001,     (int64_t)    -0x8001L, {0x8f, 0xff, 0x7f, 0xff, 0xff})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int64Test, _7fffffff,  (int64_t) 0x7fffffffL, {0x8f, 0xff, 0xff, 0xff, 0x7f})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int64Test, _n80000000, (int64_t)-0x80000000L, {0x8f, 0x00, 0x00, 0x00, 0x80})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int64Test, _80000000,  (int64_t) 0x80000000L, {0x90, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int64Test, _n80000001, (int64_t)-0x80000001L, {0x90, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int64Test, _7fffffffffffffff,  (int64_t) 0x7fffffffffffffffL, {0x90, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int64Test, _n8000000000000000, (int64_t)-0x8000000000000000L, {0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80})

DEFINE_FAILED_ADD_TEST(Int64Test, add_fail, 7, 0x7fffffffffffffffL)
