#include "encode_test_helpers.h"

DEFINE_SCALAR_WRITE_TEST(Int64Test, add_0,         (int64_t)          0L, {0x00})
DEFINE_SCALAR_WRITE_TEST(Int64Test, add_1,         (int64_t)          1L, {0x01})
DEFINE_SCALAR_WRITE_TEST(Int64Test, add_101,       (int64_t)        101L, {0x65})
DEFINE_SCALAR_WRITE_TEST(Int64Test, add_n1,        (int64_t)         -1L, {0xff})
DEFINE_SCALAR_WRITE_TEST(Int64Test, add_n101,      (int64_t)       -101L, {0x9b})
DEFINE_SCALAR_WRITE_TEST(Int64Test, add_102,       (int64_t)        102L, {TYPE_INT_16, 0x66, 0x00})
DEFINE_SCALAR_WRITE_TEST(Int64Test, add_n102,      (int64_t)       -102L, {TYPE_INT_16, 0x9a, 0xff})
DEFINE_SCALAR_WRITE_TEST(Int64Test, add_127,       (int64_t)        127L, {TYPE_INT_16, 0x7f, 0x00})
DEFINE_SCALAR_WRITE_TEST(Int64Test, add_n128,      (int64_t)       -128L, {TYPE_INT_16, 0x80, 0xff})
DEFINE_SCALAR_WRITE_TEST(Int64Test, add_7fff,      (int64_t)     0x7fffL, {TYPE_INT_16, 0xff, 0x7f})
DEFINE_SCALAR_WRITE_TEST(Int64Test, add_n8000,     (int64_t)    -0x8000L, {TYPE_INT_16, 0x00, 0x80})
DEFINE_SCALAR_WRITE_TEST(Int64Test, add_8000,      (int64_t)     0x8000L, {TYPE_INT_32, 0x00, 0x80, 0x00, 0x00})
DEFINE_SCALAR_WRITE_TEST(Int64Test, add_n8001,     (int64_t)    -0x8001L, {TYPE_INT_32, 0xff, 0x7f, 0xff, 0xff})
DEFINE_SCALAR_WRITE_TEST(Int64Test, add_7fffffff,  (int64_t) 0x7fffffffL, {TYPE_INT_32, 0xff, 0xff, 0xff, 0x7f})
DEFINE_SCALAR_WRITE_TEST(Int64Test, add_n80000000, (int64_t)-0x80000000L, {TYPE_INT_32, 0x00, 0x00, 0x00, 0x80})
DEFINE_SCALAR_WRITE_TEST(Int64Test, add_80000000,  (int64_t) 0x80000000L, {TYPE_INT_64, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00})
DEFINE_SCALAR_WRITE_TEST(Int64Test, add_n80000001, (int64_t)-0x80000001L, {TYPE_INT_64, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff})
DEFINE_SCALAR_WRITE_TEST(Int64Test, add_7fffffffffffffff,  (int64_t) 0x7fffffffffffffffL, {TYPE_INT_64, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f})
DEFINE_SCALAR_WRITE_TEST(Int64Test, add_n8000000000000000, (int64_t)-0x8000000000000000L, {TYPE_INT_64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80})
