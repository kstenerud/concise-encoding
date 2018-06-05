#include "encode_test_helpers.h"

DEFINE_ADD_VALUE_TEST(Int16Test, add_0,    (int16_t)   0, {0x00})
DEFINE_ADD_VALUE_TEST(Int16Test, add_1,    (int16_t)   1, {0x01})
DEFINE_ADD_VALUE_TEST(Int16Test, add_103,  (int16_t) 103, {0x67})
DEFINE_ADD_VALUE_TEST(Int16Test, add_n1,   (int16_t)  -1, {0xff})
DEFINE_ADD_VALUE_TEST(Int16Test, add_n104, (int16_t)-104, {0x98})
DEFINE_ADD_VALUE_TEST(Int16Test, add_104,  (int16_t) 104, {TYPE_INT_16, 0x68, 0x00})
DEFINE_ADD_VALUE_TEST(Int16Test, add_n105, (int16_t)-105, {TYPE_INT_16, 0x97, 0xff})
DEFINE_ADD_VALUE_TEST(Int16Test, add_127,  (int16_t) 127, {TYPE_INT_16, 0x7f, 0x00})
DEFINE_ADD_VALUE_TEST(Int16Test, add_n128, (int16_t)-128, {TYPE_INT_16, 0x80, 0xff})
DEFINE_ADD_VALUE_TEST(Int16Test, add_7fff,  (int16_t) 0x7fff, {TYPE_INT_16, 0xff, 0x7f})
DEFINE_ADD_VALUE_TEST(Int16Test, add_n8000, (int16_t)-0x8000, {TYPE_INT_16, 0x00, 0x80})
