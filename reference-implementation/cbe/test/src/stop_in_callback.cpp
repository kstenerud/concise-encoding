#include "test_helpers.h"

static struct cbe_decode_process* begin_decode_process()
{
    return cbe_decode_begin(get_always_false_decode_callbacks(), NULL);
}

#define DEFINE_STOP_TEST(TYPE, VALUE) \
TEST(Decode, StopInCallback_ ## TYPE) \
{ \
    struct cbe_decode_process* decode_process = begin_decode_process(); \
    const int memory_size = 1000; \
    std::array<uint8_t, memory_size> memory; \
    struct cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size()); \
    EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_add_ ## TYPE(encode_process, VALUE)); \
    int64_t buffer_length = cbe_encode_get_buffer_offset(encode_process); \
    EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_end(encode_process)); \
    EXPECT_EQ(CBE_DECODE_STATUS_STOPPED_IN_CALLBACK, cbe_decode_feed(decode_process, memory.data(), buffer_length)); \
}
DEFINE_STOP_TEST(int_8, 1);
DEFINE_STOP_TEST(int_16, 1);
DEFINE_STOP_TEST(int_32, 1);
DEFINE_STOP_TEST(int_64, 1);
DEFINE_STOP_TEST(int_128, 1);
DEFINE_STOP_TEST(float_32, 1);
DEFINE_STOP_TEST(float_64, 1);
DEFINE_STOP_TEST(float_128, 1);
DEFINE_STOP_TEST(decimal_32, 1);
DEFINE_STOP_TEST(decimal_64, 1);
DEFINE_STOP_TEST(decimal_128, 1);
DEFINE_STOP_TEST(time, 1);
DEFINE_STOP_TEST(string, "1");

#define DEFINE_ARRAY_STOP_TEST(TYPE, FUNCTION) \
TEST(Decode, StopInCallback_ ## TYPE) \
{ \
    struct cbe_decode_process* decode_process = begin_decode_process(); \
    const int memory_size = 1000; \
    std::array<uint8_t, memory_size> memory; \
    struct cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size()); \
    TYPE array[1]; \
    EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_ ## FUNCTION(encode_process, 1)); \
    EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_add_data(encode_process, array, 1)); \
    int64_t buffer_length = cbe_encode_get_buffer_offset(encode_process); \
    EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_end(encode_process)); \
    EXPECT_EQ(CBE_DECODE_STATUS_STOPPED_IN_CALLBACK, cbe_decode_feed(decode_process, memory.data(), buffer_length)); \
}
DEFINE_ARRAY_STOP_TEST(uint8_t, binary);
