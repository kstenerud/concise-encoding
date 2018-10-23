#include "test_helpers.h"

DEFINE_ENCODE_DECODE_ENCODE_TEST(Int32Test, _0,         (int32_t)          0, {0x00})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int32Test, _1,         (int32_t)          1, {0x01})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int32Test, _104,       (int32_t)        104, {0x68})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int32Test, _n1,        (int32_t)         -1, {0xff})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int32Test, _n105,      (int32_t)       -105, {0x97})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int32Test, _106,       (int32_t)        105, {0x8d, 0x69, 0x00})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int32Test, _n107,      (int32_t)       -106, {0x8d, 0x96, 0xff})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int32Test, _127,       (int32_t)        127, {0x8d, 0x7f, 0x00})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int32Test, _n128,      (int32_t)       -128, {0x8d, 0x80, 0xff})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int32Test, _7fff,      (int32_t)     0x7fff, {0x8d, 0xff, 0x7f})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int32Test, _n8000,     (int32_t)    -0x8000, {0x8d, 0x00, 0x80})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int32Test, _8000,      (int32_t)     0x8000, {0x8e, 0x00, 0x80, 0x00, 0x00})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int32Test, _n8001,     (int32_t)    -0x8001, {0x8e, 0xff, 0x7f, 0xff, 0xff})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int32Test, _7fffffff,  (int32_t) 0x7fffffff, {0x8e, 0xff, 0xff, 0xff, 0x7f})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int32Test, _n80000000, (int32_t)-0x80000000, {0x8e, 0x00, 0x00, 0x00, 0x80})

DEFINE_FAILED_ADD_TEST(Int32Test, add_fail, 3, 0x100000)
