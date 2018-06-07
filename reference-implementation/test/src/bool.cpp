#include "test_helpers.h"

DEFINE_ENCODE_DECODE_ENCODE_TEST(BoolTest, add_true,  true, {TYPE_TRUE})
DEFINE_ENCODE_DECODE_ENCODE_TEST(BoolTest, add_false, false, {TYPE_FALSE})
