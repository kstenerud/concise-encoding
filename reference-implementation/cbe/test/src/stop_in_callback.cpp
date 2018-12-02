#include "test_helpers.h"

extern "C"
{
    DEFINE_DECODE_CALLBACK_SUITE(false);
}

static cbe_decode_process* begin_decode_process(cbe_decode_callbacks* callbacks)
{
    callbacks->on_empty = on_empty;
    callbacks->on_boolean = on_boolean;
    callbacks->on_int_8 = on_int_8;
    callbacks->on_int_16 = on_int_16;
    callbacks->on_int_32 = on_int_32;
    callbacks->on_int_64 = on_int_64;
    callbacks->on_int_128 = on_int_128;
    callbacks->on_float_32 = on_float_32;
    callbacks->on_float_64 = on_float_64;
    callbacks->on_float_128 = on_float_128;
    callbacks->on_decimal_32 = on_decimal_32;
    callbacks->on_decimal_64 = on_decimal_64;
    callbacks->on_decimal_128 = on_decimal_128;
    callbacks->on_time = on_time;
    callbacks->on_end_container = on_end_container;
    callbacks->on_begin_list = on_begin_list;
    callbacks->on_begin_map = on_begin_map;
    callbacks->on_string = on_string;
    callbacks->on_binary_data = on_binary_data;
    return cbe_decode_begin(callbacks, NULL);
}

#define DEFINE_STOP_TEST(TYPE, VALUE) \
TEST(Decode, StopInCallback_ ## TYPE) \
{ \
    cbe_decode_callbacks callbacks; \
    cbe_decode_process* decode_process = begin_decode_process(&callbacks); \
    const int memory_size = 1000; \
    std::array<uint8_t, memory_size> memory; \
    cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size()); \
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
    cbe_decode_callbacks callbacks; \
    cbe_decode_process* decode_process = begin_decode_process(&callbacks); \
    const int memory_size = 1000; \
    std::array<uint8_t, memory_size> memory; \
    cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size()); \
    TYPE array[1]; \
    EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_add_ ## FUNCTION(encode_process, array, 1)); \
    int64_t buffer_length = cbe_encode_get_buffer_offset(encode_process); \
    EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_end(encode_process)); \
    EXPECT_EQ(CBE_DECODE_STATUS_STOPPED_IN_CALLBACK, cbe_decode_feed(decode_process, memory.data(), buffer_length)); \
}
DEFINE_ARRAY_STOP_TEST(uint8_t, binary_data);
