#include "test_helpers.h"

DEFINE_ENCODE_DECODE_ENCODE_TEST(BoolTest, add_true,  true, {0x79})
DEFINE_ENCODE_DECODE_ENCODE_TEST(BoolTest, add_false, false, {0x7a})

DEFINE_ADD_INCOMPLETE_TEST(BoolTest, add_incomplete_true, 0, true)
DEFINE_ADD_INCOMPLETE_TEST(BoolTest, add_incomplete_false, 0, false)
