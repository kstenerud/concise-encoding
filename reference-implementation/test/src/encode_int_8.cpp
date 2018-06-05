#include "encode_test_helpers.h"

DEFINE_ADD_VALUE_TEST(Int8Test, add_0,    (int8_t)   0, {0x00})
DEFINE_ADD_VALUE_TEST(Int8Test, add_1,    (int8_t)   1, {0x01})
DEFINE_ADD_VALUE_TEST(Int8Test, add_103,  (int8_t) 103, {0x67})
DEFINE_ADD_VALUE_TEST(Int8Test, add_n1,   (int8_t)  -1, {0xff})
DEFINE_ADD_VALUE_TEST(Int8Test, add_n104, (int8_t)-104, {0x98})
DEFINE_ADD_VALUE_TEST(Int8Test, add_104,  (int8_t) 104, {TYPE_INT_16, 0x68, 0x00})
DEFINE_ADD_VALUE_TEST(Int8Test, add_n105, (int8_t)-105, {TYPE_INT_16, 0x97, 0xff})
DEFINE_ADD_VALUE_TEST(Int8Test, add_127,  (int8_t) 127, {TYPE_INT_16, 0x7f, 0x00})
DEFINE_ADD_VALUE_TEST(Int8Test, add_n128, (int8_t)-128, {TYPE_INT_16, 0x80, 0xff})
