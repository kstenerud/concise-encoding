#pragma once

// Terrible way to bridge decimal types in C and C++
// this shit is broken
#include <decimal/decimal>
#define _Decimal64 std::decimal::decimal64
#define _Decimal128 std::decimal::decimal128

#include <functional>
#include <stdio.h>
#include <gtest/gtest.h>
#include <cbe/encoder.h>
#include <cbe_internal.h>
#include <cxxabi.h>

static cbe_buffer create_buffer(uint8_t* memory, int size)
{
    uint8_t* memory_start = &memory[0];
    uint8_t* memory_end = &memory[size];
    cbe_buffer buffer;
    cbe_init_buffer(&buffer,  memory_start, memory_end);
    return buffer;
}

template <typename T>
inline bool store_value(cbe_buffer* buffer, T value)
{
    int status;
    std::string type_name = typeid(T).name();
    char *demangled_name = abi::__cxa_demangle(type_name.c_str(), NULL, NULL, &status);
    if(status == 0) {
        type_name = demangled_name;
        std::free(demangled_name);
    }   

    std::stringstream stream;
    stream << "Unimplemented store_value handler called for " << type_name;
    throw std::runtime_error(stream.str());
}

#define DEFINE_STORE_SCALAR_FUNCTION(SCALAR_TYPE, FUNCTION_TO_CALL) \
template <> \
inline bool store_value<SCALAR_TYPE>(cbe_buffer* buffer, SCALAR_TYPE value) \
{ \
    return FUNCTION_TO_CALL(buffer, value); \
}
DEFINE_STORE_SCALAR_FUNCTION(bool,        cbe_add_boolean)
DEFINE_STORE_SCALAR_FUNCTION(int8_t,      cbe_add_int8)
DEFINE_STORE_SCALAR_FUNCTION(int16_t,     cbe_add_int16)
DEFINE_STORE_SCALAR_FUNCTION(int32_t,     cbe_add_int32)
DEFINE_STORE_SCALAR_FUNCTION(int64_t,     cbe_add_int64)
DEFINE_STORE_SCALAR_FUNCTION(__int128,    cbe_add_int128)
DEFINE_STORE_SCALAR_FUNCTION(float,       cbe_add_float32)
DEFINE_STORE_SCALAR_FUNCTION(double,      cbe_add_float64)
DEFINE_STORE_SCALAR_FUNCTION(long double, cbe_add_float128)

#define DEFINE_STORE_VECTOR_FUNCTION(VECTOR_TYPE, FUNCTION_TO_CALL) \
template <> \
inline bool store_value<std::vector<VECTOR_TYPE>>(cbe_buffer* buffer, std::vector<VECTOR_TYPE> value) \
{ \
    return FUNCTION_TO_CALL(buffer, value.data(), value.size()); \
}
DEFINE_STORE_VECTOR_FUNCTION(uint8_t, cbe_add_bytes)
DEFINE_STORE_VECTOR_FUNCTION(int16_t, cbe_add_int16_array)
DEFINE_STORE_VECTOR_FUNCTION(int32_t, cbe_add_int32_array)
DEFINE_STORE_VECTOR_FUNCTION(int64_t, cbe_add_int64_array)
DEFINE_STORE_VECTOR_FUNCTION(__int128, cbe_add_int128_array)
DEFINE_STORE_VECTOR_FUNCTION(float, cbe_add_float32_array)
DEFINE_STORE_VECTOR_FUNCTION(double, cbe_add_float64_array)
DEFINE_STORE_VECTOR_FUNCTION(long double, cbe_add_float128_array)

template <>
inline bool store_value<std::string>(cbe_buffer* buffer, std::string value)
{
    return cbe_add_string(buffer, value.c_str());
}

inline void expect_memory_after_store_function(std::function<bool(cbe_buffer* buffer)> store_function,
                                                std::vector<uint8_t> const& expected_memory)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;
    uint8_t* data = memory.data();
    memset(data, 0xf7, memory_size);
    int expected_size = expected_memory.size();
    uint8_t* expected_pos = data + expected_size;

    cbe_buffer buffer = create_buffer(data, memory_size);
    bool success = store_function(&buffer);
    fflush(stdout);

    std::vector<uint8_t> actual_memory = std::vector<uint8_t>(data, data + expected_size);
    EXPECT_TRUE(success);
    EXPECT_EQ(expected_pos, buffer.pos);
    EXPECT_EQ(expected_memory, actual_memory);
}

template<typename T>
inline void expect_memory_after_write(T writeValue, std::vector<uint8_t> const& expected_memory)
{
    expect_memory_after_store_function([&](cbe_buffer* buffer) {return store_value(buffer, writeValue);}, expected_memory);
}

#define DEFINE_SCALAR_WRITE_TEST(TESTCASE, NAME, VALUE, ...) \
TEST(TESTCASE, NAME) \
{ \
    expect_memory_after_write(VALUE, __VA_ARGS__); \
}
