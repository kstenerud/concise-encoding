#include "test_helpers.h"

DEFINE_ENCODE_DECODE_ENCODE_TEST(BoolTest, add_true,  true, {TYPE_TRUE})
DEFINE_ENCODE_DECODE_ENCODE_TEST(BoolTest, add_false, false, {TYPE_FALSE})

DEFINE_FAILED_ADD_TEST(BoolTest, add_fail_true, 0, true)
DEFINE_FAILED_ADD_TEST(BoolTest, add_fail_false, 0, false)
