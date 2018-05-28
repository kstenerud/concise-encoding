#pragma once

#include <stdio.h>
#include <gtest/gtest.h>
#include <cbe/cbe.h>
#include <cbe_internal.h>

static cbe_buffer create_buffer(uint8_t* memory, int size)
{
    uint8_t* memory_start = &memory[0];
    uint8_t* memory_end = &memory[size];
    cbe_buffer buffer;
    cbe_init_buffer(&buffer,  memory_start, memory_end);
    return buffer;
}

template <typename T>
inline bool store_value(cbe_buffer* buffer, T value) {return false;}

#define DEFINE_STORE_VALUE(TYPE, FUNCTION) \
template <> \
inline bool store_value<TYPE>(cbe_buffer* buffer, TYPE value) \
{ \
    return FUNCTION(buffer, value); \
}

DEFINE_STORE_VALUE(bool, cbe_add_boolean)
DEFINE_STORE_VALUE(int8_t, cbe_add_int8)
DEFINE_STORE_VALUE(int16_t, cbe_add_int16)
DEFINE_STORE_VALUE(int32_t, cbe_add_int32)
DEFINE_STORE_VALUE(int64_t, cbe_add_int64)

template<typename T>
inline void scalar_write_test(T value, std::vector<uint8_t>& expected_memory)
{
	int expected_size = expected_memory.size();
	const int memory_size = 100;
	std::array<uint8_t, memory_size> memory;
	uint8_t* data = memory.data();
    uint8_t* expected_pos = data + expected_size;

    cbe_buffer buffer = create_buffer(data, memory_size);
    bool success = store_value(&buffer, value);

    std::vector<uint8_t> actual_memory = std::vector<uint8_t>(data, data + expected_size);
    EXPECT_TRUE(success);
    EXPECT_EQ(expected_pos, buffer.pos);
    EXPECT_EQ(expected_memory, actual_memory);
}

#define DEFINE_SCALAR_WRITE_TEST(TESTCASE, NAME, VALUE, ...) \
TEST(TESTCASE, NAME) \
{ \
	uint8_t expected[] = __VA_ARGS__; \
	std::vector<uint8_t> vec(expected, expected + sizeof(expected) / sizeof(expected[0])); \
	scalar_write_test(VALUE, vec); \
}
