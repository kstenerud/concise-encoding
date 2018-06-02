#include "encode_test_helpers.h"

static void expect_memory_after_add_map(int length, std::vector<uint8_t> expected_memory)
{
	expect_memory_after_add_function([=](cbe_buffer* buffer)
	{
		if(!cbe_start_map(buffer)) return false;
		for(int i = 0; i < length; i++)
		{
			char stringbuffer[2] = {0};
			stringbuffer[0] = 'a' + i;
			if(!cbe_add_int8(buffer, i + 1)) return false;
			if(!cbe_add_string(buffer, stringbuffer)) return false;
		}
		return cbe_end_container(buffer);
	}, expected_memory);
}

#define DEFINE_ADD_MAP_TEST(LENGTH, ...) \
TEST(MapTest, length_ ## LENGTH) \
{ \
    expect_memory_after_add_map(LENGTH, __VA_ARGS__); \
}

DEFINE_ADD_MAP_TEST(0, {TYPE_MAP, TYPE_END_CONTAINER})
DEFINE_ADD_MAP_TEST(1, {TYPE_MAP, 1, TYPE_STRING_1, 'a', TYPE_END_CONTAINER})
DEFINE_ADD_MAP_TEST(2, {TYPE_MAP, 1, TYPE_STRING_1, 'a', 2, TYPE_STRING_1, 'b', TYPE_END_CONTAINER})
DEFINE_ADD_MAP_TEST(3, {TYPE_MAP, 1, TYPE_STRING_1, 'a', 2, TYPE_STRING_1, 'b', 3, TYPE_STRING_1, 'c', TYPE_END_CONTAINER})
