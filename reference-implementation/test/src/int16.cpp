#include "test_helpers.h"

DEFINE_SCALAR_WRITE_TEST(Int16Test, add_0,    (int16_t)   0, {0x00})
DEFINE_SCALAR_WRITE_TEST(Int16Test, add_1,    (int16_t)   1, {0x01})
DEFINE_SCALAR_WRITE_TEST(Int16Test, add_100,  (int16_t) 100, {0x64})
DEFINE_SCALAR_WRITE_TEST(Int16Test, add_n1,   (int16_t)  -1, {0xff})
DEFINE_SCALAR_WRITE_TEST(Int16Test, add_n100, (int16_t)-100, {0x9c})
DEFINE_SCALAR_WRITE_TEST(Int16Test, add_101,  (int16_t) 101, {TYPE_INT_16, 0x65, 0x00})
DEFINE_SCALAR_WRITE_TEST(Int16Test, add_n101, (int16_t)-101, {TYPE_INT_16, 0x9b, 0xff})
DEFINE_SCALAR_WRITE_TEST(Int16Test, add_127,  (int16_t) 127, {TYPE_INT_16, 0x7f, 0x00})
DEFINE_SCALAR_WRITE_TEST(Int16Test, add_n128, (int16_t)-128, {TYPE_INT_16, 0x80, 0xff})
DEFINE_SCALAR_WRITE_TEST(Int16Test, add_7fff,  (int16_t) 0x7fff, {TYPE_INT_16, 0xff, 0x7f})
DEFINE_SCALAR_WRITE_TEST(Int16Test, add_n8000, (int16_t)-0x8000, {TYPE_INT_16, 0x00, 0x80})
