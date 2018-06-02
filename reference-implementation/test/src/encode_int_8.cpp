#include "encode_test_helpers.h"

DEFINE_ADD_VALUE_TEST(Int8Test, add_0,    (int8_t)   0, {0x00})
DEFINE_ADD_VALUE_TEST(Int8Test, add_1,    (int8_t)   1, {0x01})
DEFINE_ADD_VALUE_TEST(Int8Test, add_101,  (int8_t) 101, {0x65})
DEFINE_ADD_VALUE_TEST(Int8Test, add_n1,   (int8_t)  -1, {0xff})
DEFINE_ADD_VALUE_TEST(Int8Test, add_n101, (int8_t)-101, {0x9b})
DEFINE_ADD_VALUE_TEST(Int8Test, add_102,  (int8_t) 102, {TYPE_INT_16, 0x66, 0x00})
DEFINE_ADD_VALUE_TEST(Int8Test, add_n102, (int8_t)-102, {TYPE_INT_16, 0x9a, 0xff})
DEFINE_ADD_VALUE_TEST(Int8Test, add_127,  (int8_t) 127, {TYPE_INT_16, 0x7f, 0x00})
DEFINE_ADD_VALUE_TEST(Int8Test, add_n128, (int8_t)-128, {TYPE_INT_16, 0x80, 0xff})
