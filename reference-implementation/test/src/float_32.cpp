#include "test_helpers.h"

DEFINE_ENCODE_DECODE_ENCODE_TEST(Float32Test, add_0,                   0.0f, {0x91, 0x00, 0x00, 0x00, 0x00})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Float32Test, add_0_1,                 0.1f, {0x91, 0xcd, 0xcc, 0xcc, 0x3d})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Float32Test, add_0_000014,       0.000014f, {0x91, 0x8b, 0xe1, 0x6a, 0x37})
DEFINE_ENCODE_DECODE_ENCODE_TEST(Float32Test, add_n967234_125, -967234.125f, {0x91, 0x22, 0x24, 0x6c, 0xc9})

DEFINE_FAILED_ADD_TEST(Float32Test, add_fail, 7, 0.1f)
