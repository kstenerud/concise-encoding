#include "old_test_helpers.h"

static void expect_memory_after_add_map(int length, std::vector<uint8_t> expected_memory)
{
    expect_memory_after_operation([=](struct cbe_encode_process* encode_process)
    {
        cbe_encode_status status;
        if((status = cbe_encode_begin_map(encode_process)) != CBE_ENCODE_STATUS_OK) return status;
        for(int i = 0; i < length; i++)
        {
            char stringbuffer[2] = {0};
            stringbuffer[0] = 'a' + i;
            if((status = cbe_encode_add_int_8(encode_process, i + 1)) != CBE_ENCODE_STATUS_OK) return status;
            if((status = cbe_encode_add_string(encode_process, stringbuffer)) != CBE_ENCODE_STATUS_OK) return status;
        }
        return cbe_encode_end_container(encode_process);
    }, expected_memory);
}

#define DEFINE_ADD_MAP_TEST(LENGTH, ...) \
TEST(Map, length_ ## LENGTH) \
{ \
    std::vector<uint8_t> expected_memory = __VA_ARGS__; \
    expect_memory_after_add_map(LENGTH, expected_memory); \
    expect_decode_encode(expected_memory); \
}

DEFINE_ADD_MAP_TEST(0, {0x7c, 0x7d})
DEFINE_ADD_MAP_TEST(1, {0x7c, 1, 0x81, 'a', 0x7d})
DEFINE_ADD_MAP_TEST(2, {0x7c, 1, 0x81, 'a', 2, 0x81, 'b', 0x7d})
DEFINE_ADD_MAP_TEST(3, {0x7c, 1, 0x81, 'a', 2, 0x81, 'b', 3, 0x81, 'c', 0x7d})

TEST(Map, string_key) \
{
    std::vector<uint8_t> expected_memory = {0x7c, 0x81, 'a', 0x02, 0x7d};
    expect_decode_encode(expected_memory);
}

TEST(Map, incomplete)
{
    expect_incomplete_operation_decrementing(3, [&](struct cbe_encode_process* encode_process)
    {
        cbe_encode_status status;
        if((status = cbe_encode_begin_list(encode_process)) != CBE_ENCODE_STATUS_OK) return status;
        if((status = cbe_encode_add_int_8(encode_process, 0)) != CBE_ENCODE_STATUS_OK) return status;
        if((status = cbe_encode_add_int_8(encode_process, 0)) != CBE_ENCODE_STATUS_OK) return status;
        return cbe_encode_end_container(encode_process);
    });
}
