#include "encode_test_helpers.h"

DEFINE_SCALAR_WRITE_TEST(Int8Test, add_0,    (int8_t)   0, {0x00})
DEFINE_SCALAR_WRITE_TEST(Int8Test, add_1,    (int8_t)   1, {0x01})
DEFINE_SCALAR_WRITE_TEST(Int8Test, add_100,  (int8_t) 100, {0x64})
DEFINE_SCALAR_WRITE_TEST(Int8Test, add_n1,   (int8_t)  -1, {0xff})
DEFINE_SCALAR_WRITE_TEST(Int8Test, add_n100, (int8_t)-100, {0x9c})
DEFINE_SCALAR_WRITE_TEST(Int8Test, add_101,  (int8_t) 101, {TYPE_INT_16, 0x65, 0x00})
DEFINE_SCALAR_WRITE_TEST(Int8Test, add_n101, (int8_t)-101, {TYPE_INT_16, 0x9b, 0xff})
DEFINE_SCALAR_WRITE_TEST(Int8Test, add_127,  (int8_t) 127, {TYPE_INT_16, 0x7f, 0x00})
DEFINE_SCALAR_WRITE_TEST(Int8Test, add_n128, (int8_t)-128, {TYPE_INT_16, 0x80, 0xff})
