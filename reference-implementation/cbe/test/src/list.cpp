#include "test_helpers.h"

static void expect_memory_after_add_list(int length, std::vector<uint8_t> expected_memory)
{
    expect_memory_after_operation([=](cbe_encode_process* encode_process)
    {
        if(!cbe_encode_start_list(encode_process)) return false;
        for(int i = 0; i < length; i++)
        {
            int value = 10; // Any old number
            if(!cbe_encode_add_int_8(encode_process, value)) return false;
        }
        return cbe_end_container(encode_process);
    }, expected_memory);
}

#define DEFINE_ADD_LIST_TEST(LENGTH, ...) \
TEST(ListTest, length_ ## LENGTH) \
{ \
    std::vector<uint8_t> expected_memory = __VA_ARGS__; \
    expect_memory_after_add_list(LENGTH, expected_memory); \
    expect_decode_encode(expected_memory); \
}

DEFINE_ADD_LIST_TEST(0, {0x91, 0x93})
DEFINE_ADD_LIST_TEST(1, {0x91, 10, 0x93})
DEFINE_ADD_LIST_TEST(2, {0x91, 10, 10, 0x93})
DEFINE_ADD_LIST_TEST(3, {0x91, 10, 10, 10, 0x93})
DEFINE_ADD_LIST_TEST(4, {0x91, 10, 10, 10, 10, 0x93})
DEFINE_ADD_LIST_TEST(5, {0x91, 10, 10, 10, 10, 10, 0x93})

TEST(ListTest, failed)
{
    expect_failed_operation_decrementing(2, [&](cbe_encode_process* encode_process)
    {
        cbe_encode_start_list(encode_process);
        cbe_encode_add_int_8(encode_process, 0);
        return cbe_end_container(encode_process);
    });
}
