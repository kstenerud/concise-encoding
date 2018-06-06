#pragma once

#include <functional>
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

template<typename T>
static std::vector<T> make_values_of_length(int length)
{
    std::vector<T> vec;
    for(int i = 0; i < length; i++)
    {
        vec.push_back((T)(i & 0x7f));
    }
    return vec;
}

inline void expect_memory_after_operation(std::function<bool(cbe_buffer* buffer)> operation,
                                          std::vector<uint8_t> const& expected_memory)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;
    uint8_t* data = memory.data();
    memset(data, 0xf7, memory_size);
    int expected_size = expected_memory.size();
    uint8_t* expected_pos = data + expected_size;

    cbe_buffer buffer = create_buffer(data, memory_size);
    bool success = operation(&buffer);
    fflush(stdout);

    std::vector<uint8_t> actual_memory = std::vector<uint8_t>(data, data + expected_size);
    EXPECT_TRUE(success);
    EXPECT_EQ(expected_pos, buffer.pos);
    EXPECT_EQ(expected_memory, actual_memory);
}

