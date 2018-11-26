#pragma once

#include <functional>
#include <stdio.h>
#include <gtest/gtest.h>
#include <cbe/cbe.h>
#include "decode_encode.h"


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

inline void expect_memory_after_operation(std::function<bool(cbe_encode_process* encode_process)> operation,
                                          std::vector<uint8_t> const& expected_memory)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;
    uint8_t* data = memory.data();
    memset(data, 0xf7, memory_size);
    int expected_size = expected_memory.size();

    cbe_encode_process* encode_process = cbe_encode_begin(data, memory_size);
    bool success = operation(encode_process);
    fflush(stdout);

    std::vector<uint8_t> actual_memory = std::vector<uint8_t>(data, data + expected_size);
    EXPECT_TRUE(success);
    EXPECT_EQ(expected_size, cbe_encode_get_buffer_offset(encode_process));
    EXPECT_EQ(expected_memory, actual_memory);
}

inline void expect_failed_operation(const int buffer_size, std::function<bool(cbe_encode_process* encode_process)> operation)
{
    uint8_t data[buffer_size];
    memset(data, 0xf7, buffer_size);
    cbe_encode_process* encode_process = cbe_encode_begin(data, buffer_size);
    bool success = operation(encode_process);
    fflush(stdout);
    EXPECT_FALSE(success);
}

inline void expect_failed_operation_decrementing(const int buffer_size, std::function<bool(cbe_encode_process* encode_process)> operation)
{
    for(int size = buffer_size; size >= 0; size--)
    {
        expect_failed_operation(size, operation);
    }
}

// Internal

template <typename T>
inline bool add_value(cbe_encode_process* encode_process, T value)
{
    int status;
    std::string type_name = typeid(T).name();
    char *demangled_name = abi::__cxa_demangle(type_name.c_str(), NULL, NULL, &status);
    if(status == 0) {
        type_name = demangled_name;
        std::free(demangled_name);
    }   

    std::stringstream stream;
    stream << "Unimplemented add_value handler called for " << type_name;
    throw std::runtime_error(stream.str());
}

#define DEFINE_ADD_VALUE_FUNCTION(SCALAR_TYPE, FUNCTION_TO_CALL) \
template <> \
inline bool add_value<SCALAR_TYPE>(cbe_encode_process* encode_process, SCALAR_TYPE value) \
{ \
    return FUNCTION_TO_CALL(encode_process, value); \
}
DEFINE_ADD_VALUE_FUNCTION(bool,        cbe_encode_add_boolean)
DEFINE_ADD_VALUE_FUNCTION(int8_t,      cbe_encode_add_int_8)
DEFINE_ADD_VALUE_FUNCTION(int16_t,     cbe_encode_add_int_16)
DEFINE_ADD_VALUE_FUNCTION(int32_t,     cbe_encode_add_int_32)
DEFINE_ADD_VALUE_FUNCTION(int64_t,     cbe_encode_add_int_64)
DEFINE_ADD_VALUE_FUNCTION(__int128,    cbe_encode_add_int_128)
DEFINE_ADD_VALUE_FUNCTION(float,       cbe_encode_add_float_32)
DEFINE_ADD_VALUE_FUNCTION(double,      cbe_encode_add_float_64)
DEFINE_ADD_VALUE_FUNCTION(__float128, cbe_encode_add_float_128)
DEFINE_ADD_VALUE_FUNCTION(_Decimal32,  cbe_encode_add_decimal_32)
DEFINE_ADD_VALUE_FUNCTION(_Decimal64,  cbe_encode_add_decimal_64)
DEFINE_ADD_VALUE_FUNCTION(_Decimal128, cbe_encode_add_decimal_128)
// DEFINE_ADD_VALUE_FUNCTION(int64_t,     cbe_encode_add_time)

#define DEFINE_ADD_VECTOR_FUNCTION(VECTOR_TYPE, FUNCTION_TO_CALL) \
template <> \
inline bool add_value<std::vector<VECTOR_TYPE>>(cbe_encode_process* encode_process, std::vector<VECTOR_TYPE> value) \
{ \
    return FUNCTION_TO_CALL(encode_process, value.data(), value.size()); \
}
DEFINE_ADD_VECTOR_FUNCTION(int8_t,      cbe_encode_add_array_int_8)
DEFINE_ADD_VECTOR_FUNCTION(int16_t,     cbe_encode_add_array_int_16)
DEFINE_ADD_VECTOR_FUNCTION(int32_t,     cbe_encode_add_array_int_32)
DEFINE_ADD_VECTOR_FUNCTION(int64_t,     cbe_encode_add_array_int_64)
DEFINE_ADD_VECTOR_FUNCTION(__int128,    cbe_encode_add_array_int_128)
DEFINE_ADD_VECTOR_FUNCTION(float,       cbe_encode_add_array_float_32)
DEFINE_ADD_VECTOR_FUNCTION(double,      cbe_encode_add_array_float_64)
DEFINE_ADD_VECTOR_FUNCTION(__float128, cbe_encode_add_array_float_128)
DEFINE_ADD_VECTOR_FUNCTION(_Decimal32,  cbe_encode_add_array_decimal_32)
DEFINE_ADD_VECTOR_FUNCTION(_Decimal64,  cbe_encode_add_array_decimal_64)
DEFINE_ADD_VECTOR_FUNCTION(_Decimal128, cbe_encode_add_array_decimal_128)
// DEFINE_ADD_VECTOR_FUNCTION(int64_t,     cbe_encode_add_array_time)

template <>
inline bool add_value<std::string>(cbe_encode_process* encode_process, std::string value)
{
    return cbe_encode_add_string(encode_process, value.c_str());
}


template <>
inline bool add_value<std::vector<bool>>(cbe_encode_process* encode_process, std::vector<bool> entries)
{
    bool array[entries.size()];
    int index = 0;
    for(bool entry: entries)
    {
        array[index++] = entry;
    }
    return cbe_encode_add_array_boolean(encode_process, array, sizeof(array));
}


// Test functions

template<typename T>
static void add_bytes(std::vector<uint8_t>& bytes, T value)
{
    int8_t* ptr = (int8_t*)&value;
    bytes.insert(bytes.end(), ptr, ptr+sizeof(value));
}

template<typename T>
inline void expect_memory_after_add_value(T writeValue, std::vector<uint8_t> const& expected_memory)
{
    expect_memory_after_operation([&](cbe_encode_process* encode_process) {return add_value(encode_process, writeValue);}, expected_memory);
}

inline void expect_decode_encode(std::vector<uint8_t> const& expected_memory)
{
    expect_memory_after_operation([=](cbe_encode_process* encode_process)
    {
        return decode_encode(expected_memory.data(), expected_memory.size(), encode_process);
    },
     expected_memory);
}

template<typename T>
inline void expect_add_value_fail(const int buffer_size, T writeValue)
{
    expect_failed_operation(buffer_size, [&](cbe_encode_process* encode_process) \
    { \
        return add_value(encode_process, writeValue); \
    }); \
}

template<typename T>
inline void expect_add_value_fail_decrementing(const int buffer_size, T writeValue)
{
    expect_failed_operation_decrementing(buffer_size, [&](cbe_encode_process* encode_process) \
    { \
        return add_value(encode_process, writeValue); \
    }); \
}

#define DEFINE_ADD_VALUE_TEST(TESTCASE, NAME, VALUE, ...) \
TEST(TESTCASE, NAME) \
{ \
    expect_memory_after_add_value(VALUE, __VA_ARGS__); \
}

#define DEFINE_DECODE_ENCODE_TEST(TESTCASE, NAME, ...) \
TEST(TESTCASE, NAME) \
{ \
    expect_decode_encode(__VA_ARGS__); \
}

#define DEFINE_ENCODE_DECODE_ENCODE_TEST(TESTCASE, NAME, VALUE, ...) \
TEST(TESTCASE, NAME) \
{ \
    const std::vector<uint8_t> expected_memory = __VA_ARGS__; \
    expect_memory_after_add_value(VALUE, expected_memory); \
    expect_decode_encode(expected_memory); \
}

#define DEFINE_FAILED_ADD_TEST(TESTCASE, NAME, SIZE, VALUE) \
TEST(TESTCASE, NAME) \
{ \
    expect_add_value_fail_decrementing(SIZE, VALUE); \
}
