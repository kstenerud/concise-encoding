#include "encode_test_helpers.h"

TEST(EmptyTest, empty)
{
	expect_memory_after_add_function([](cbe_encode_buffer* buffer) {
		return cbe_add_empty(buffer);
	}, {TYPE_EMPTY});
}
