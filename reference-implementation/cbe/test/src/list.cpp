#include "test_helpers.h"

static void expect_memory_after_add_list(int length, std::vector<uint8_t> expected_memory)
{
    expect_memory_after_operation([=](cbe_buffer* buffer)
    {
        if(!cbe_start_list(buffer)) return false;
        for(int i = 0; i < length; i++)
        {
            int value = 10; // Any old number
            if(!cbe_add_int_8(buffer, value)) return false;
        }
        return cbe_end_container(buffer);
    }, expected_memory);
}

#define DEFINE_ADD_LIST_TEST(LENGTH, ...) \
TEST(ListTest, length_ ## LENGTH) \
{ \
    std::vector<uint8_t> expected_memory = __VA_ARGS__; \
    expect_memory_after_add_list(LENGTH, expected_memory); \
    expect_decode_encode(expected_memory); \
}

DEFINE_ADD_LIST_TEST(0, {0x6b, 0x6d})
DEFINE_ADD_LIST_TEST(1, {0x6b, 10, 0x6d})
DEFINE_ADD_LIST_TEST(2, {0x6b, 10, 10, 0x6d})
DEFINE_ADD_LIST_TEST(3, {0x6b, 10, 10, 10, 0x6d})
DEFINE_ADD_LIST_TEST(4, {0x6b, 10, 10, 10, 10, 0x6d})
DEFINE_ADD_LIST_TEST(5, {0x6b, 10, 10, 10, 10, 10, 0x6d})

TEST(ListTest, failed)
{
    expect_failed_operation_decrementing(2, [&](cbe_buffer* buffer)
    {
        cbe_start_list(buffer);
        cbe_add_int_8(buffer, 0);
        return cbe_end_container(buffer);
    });
}
