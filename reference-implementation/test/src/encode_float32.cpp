#include "encode_test_helpers.h"

DEFINE_ADD_VALUE_TEST(Float32Test, add_0,                   0.0f, {TYPE_FLOAT_32, 0x00, 0x00, 0x00, 0x00})
DEFINE_ADD_VALUE_TEST(Float32Test, add_0_1,                 0.1f, {TYPE_FLOAT_32, 0xcd, 0xcc, 0xcc, 0x3d})
DEFINE_ADD_VALUE_TEST(Float32Test, add_0_000014,       0.000014f, {TYPE_FLOAT_32, 0x8b, 0xe1, 0x6a, 0x37})
DEFINE_ADD_VALUE_TEST(Float32Test, add_n967234_125, -967234.125f, {TYPE_FLOAT_32, 0x22, 0x24, 0x6c, 0xc9})
