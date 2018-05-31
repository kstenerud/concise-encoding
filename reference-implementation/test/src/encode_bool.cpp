#include "encode_test_helpers.h"

DEFINE_SCALAR_WRITE_TEST(BoolTest, add_true,  true, {TYPE_TRUE})
DEFINE_SCALAR_WRITE_TEST(BoolTest, add_false, false, {TYPE_FALSE})
