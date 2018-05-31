#include "encode_test_helpers.h"

DEFINE_SCALAR_WRITE_TEST(Float64Test, add_0,   0.0, {TYPE_FLOAT_32, 0x00, 0x00, 0x00, 0x00})
DEFINE_SCALAR_WRITE_TEST(Float64Test, add_0_5, 0.5, {TYPE_FLOAT_32, 0x00, 0x00, 0x00, 0x3f})
DEFINE_SCALAR_WRITE_TEST(Float64Test, add_0_1, 0.1, {TYPE_FLOAT_64, 0x9a, 0x99, 0x99, 0x99, 0x99, 0x99, 0xb9, 0x3f})
