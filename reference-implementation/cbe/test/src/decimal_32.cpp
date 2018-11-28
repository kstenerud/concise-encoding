#include "test_helpers.h"

DEFINE_ENCODE_DECODE_ENCODE_TEST(Decimal32Test, add_0,   0.0df, {0x6f, 0x00, 0x00, 0x00, 0x32})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Decimal32Test, add_0_5, 0.5df, {0x6f, 0x05, 0x00, 0x00, 0x32})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Decimal32Test, add_0_1, 0.1df, {0x6f, 0x01, 0x00, 0x00, 0x32})
