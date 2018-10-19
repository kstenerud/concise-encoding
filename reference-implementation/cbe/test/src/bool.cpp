#include "test_helpers.h"

DEFINE_ENCODE_DECODE_ENCODE_TEST(BoolTest, add_true,  true, {0x69})
DEFINE_ENCODE_DECODE_ENCODE_TEST(BoolTest, add_false, false, {0x68})

DEFINE_FAILED_ADD_TEST(BoolTest, add_fail_true, 0, true)
DEFINE_FAILED_ADD_TEST(BoolTest, add_fail_false, 0, false)
