#include "test_helpers.h"

DEFINE_ENCODE_DECODE_ENCODE_TEST(Int16Test, _0,     (int16_t)      0, {0x00})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int16Test, _1,     (int16_t)      1, {0x01})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int16Test, _103,   (int16_t)    103, {0x67})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int16Test, _n1,    (int16_t)     -1, {0xff})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int16Test, _n104,  (int16_t)   -104, {0x98})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int16Test, _104,   (int16_t)    104, {0x8e, 0x68, 0x00})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int16Test, _n105,  (int16_t)   -105, {0x8e, 0x97, 0xff})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int16Test, _127,   (int16_t)    127, {0x8e, 0x7f, 0x00})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int16Test, _n128,  (int16_t)   -128, {0x8e, 0x80, 0xff})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int16Test, _7fff,  (int16_t) 0x7fff, {0x8e, 0xff, 0x7f})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Int16Test, _n8000, (int16_t)-0x8000, {0x8e, 0x00, 0x80})

DEFINE_FAILED_ADD_TEST(Int16Test, add_fail, 1, 0x1000)