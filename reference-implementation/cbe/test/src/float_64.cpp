#include "test_helpers.h"

DEFINE_ENCODE_DECODE_ENCODE_TEST(Float64Test, add_0,   0.0, {0x92, 0x00, 0x00, 0x00, 0x00})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Float64Test, add_0_5, 0.5, {0x92, 0x00, 0x00, 0x00, 0x3f})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Float64Test, add_0_1, 0.1, {0x93, 0x9a, 0x99, 0x99, 0x99, 0x99, 0x99, 0xb9, 0x3f})
