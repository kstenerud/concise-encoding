#include "encode_test_helpers.h"

static void test_add_list(int length, std::vector<uint8_t> expected_memory)
{
	expect_memory_after_store_function([=](cbe_buffer* buffer)
	{
		if(!cbe_start_list(buffer)) return false;
		for(int i = 0; i < length; i++)
		{
			if(!cbe_add_int8(buffer, 10)) return false;
		}
		return cbe_end_container(buffer);
	}, expected_memory);
}

#define DEFINE_ADD_LIST_TEST(LENGTH, ...) \
TEST(ListTest, length_ ## LENGTH) \
{ \
    test_add_list(LENGTH, __VA_ARGS__); \
}

DEFINE_ADD_LIST_TEST(0, {TYPE_LIST, TYPE_END_CONTAINER})
DEFINE_ADD_LIST_TEST(1, {TYPE_LIST, 10, TYPE_END_CONTAINER})
DEFINE_ADD_LIST_TEST(2, {TYPE_LIST, 10, 10, TYPE_END_CONTAINER})
DEFINE_ADD_LIST_TEST(3, {TYPE_LIST, 10, 10, 10, TYPE_END_CONTAINER})
DEFINE_ADD_LIST_TEST(4, {TYPE_LIST, 10, 10, 10, 10, TYPE_END_CONTAINER})
DEFINE_ADD_LIST_TEST(5, {TYPE_LIST, 10, 10, 10, 10, 10, TYPE_END_CONTAINER})
