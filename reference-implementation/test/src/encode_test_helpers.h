#pragma once

#include <functional>
#include <stdio.h>
#include <gtest/gtest.h>
#include <cbe/cbe.h>
#include <cbe_internal.h>
#include <cxxabi.h>

// Internal

static cbe_buffer create_buffer(uint8_t* memory, int size)
{
    uint8_t* memory_start = &memory[0];
    uint8_t* memory_end = &memory[size];
    cbe_buffer buffer;
    cbe_init_buffer(&buffer,  memory_start, memory_end);
    return buffer;
}

template <typename T>
inline bool add_value(cbe_buffer* buffer, T value)
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
inline bool add_value<SCALAR_TYPE>(cbe_buffer* buffer, SCALAR_TYPE value) \
{ \
    return FUNCTION_TO_CALL(buffer, value); \
}
DEFINE_ADD_VALUE_FUNCTION(bool,        cbe_add_boolean)
DEFINE_ADD_VALUE_FUNCTION(int8_t,      cbe_add_int_8)
DEFINE_ADD_VALUE_FUNCTION(int16_t,     cbe_add_int_16)
DEFINE_ADD_VALUE_FUNCTION(int32_t,     cbe_add_int_32)
DEFINE_ADD_VALUE_FUNCTION(int64_t,     cbe_add_int_64)
DEFINE_ADD_VALUE_FUNCTION(__int128,    cbe_add_int_128)
DEFINE_ADD_VALUE_FUNCTION(float,       cbe_add_float_32)
DEFINE_ADD_VALUE_FUNCTION(double,      cbe_add_float_64)
DEFINE_ADD_VALUE_FUNCTION(long double, cbe_add_float_128)
// DEFINE_ADD_VALUE_FUNCTION(_Decimal64,  cbe_add_decimal_64)
// DEFINE_ADD_VALUE_FUNCTION(_Decimal128, cbe_add_decimal_128)

#define DEFINE_ADD_VECTOR_FUNCTION(VECTOR_TYPE, FUNCTION_TO_CALL) \
template <> \
inline bool add_value<std::vector<VECTOR_TYPE>>(cbe_buffer* buffer, std::vector<VECTOR_TYPE> value) \
{ \
    return FUNCTION_TO_CALL(buffer, value.data(), value.size()); \
}
DEFINE_ADD_VECTOR_FUNCTION(uint8_t,     cbe_add_bytes)
DEFINE_ADD_VECTOR_FUNCTION(int16_t,     cbe_add_array_int_16)
DEFINE_ADD_VECTOR_FUNCTION(int32_t,     cbe_add_array_int_32)
DEFINE_ADD_VECTOR_FUNCTION(int64_t,     cbe_add_array_int_64)
DEFINE_ADD_VECTOR_FUNCTION(__int128,    cbe_add_array_int_128)
DEFINE_ADD_VECTOR_FUNCTION(float,       cbe_add_array_float_32)
DEFINE_ADD_VECTOR_FUNCTION(double,      cbe_add_array_float_64)
DEFINE_ADD_VECTOR_FUNCTION(long double, cbe_add_array_float_128)
// DEFINE_ADD_VECTOR_FUNCTION(_Decimal64,  cbe_add_array_decimal_64)
// DEFINE_ADD_VECTOR_FUNCTION(_Decimal128, cbe_add_array_decimal_128)

template <>
inline bool add_value<std::string>(cbe_buffer* buffer, std::string value)
{
    return cbe_add_string(buffer, value.c_str());
}


template <>
inline bool add_value<std::vector<bool>>(cbe_buffer* buffer, std::vector<bool> entries)
{
    bool array[entries.size()];
    int index = 0;
    for(bool entry: entries)
    {
        array[index++] = entry;
    }
    return cbe_add_array_boolean(buffer, array, entries.size());
}


// Test functions

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

template<typename T>
static void add_bytes(std::vector<uint8_t>& bytes, T value)
{
    int8_t* ptr = (int8_t*)&value;
    bytes.insert(bytes.end(), ptr, ptr+sizeof(value));
}

inline void expect_memory_after_add_function(std::function<bool(cbe_buffer* buffer)> add_function,
                                             std::vector<uint8_t> const& expected_memory)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;
    uint8_t* data = memory.data();
    memset(data, 0xf7, memory_size);
    int expected_size = expected_memory.size();
    uint8_t* expected_pos = data + expected_size;

    cbe_buffer buffer = create_buffer(data, memory_size);
    bool success = add_function(&buffer);
    fflush(stdout);

    std::vector<uint8_t> actual_memory = std::vector<uint8_t>(data, data + expected_size);
    EXPECT_TRUE(success);
    EXPECT_EQ(expected_pos, buffer.pos);
    EXPECT_EQ(expected_memory, actual_memory);
}

template<typename T>
inline void expect_memory_after_add_value(T writeValue, std::vector<uint8_t> const& expected_memory)
{
    expect_memory_after_add_function([&](cbe_buffer* buffer) {return add_value(buffer, writeValue);}, expected_memory);
}

#define DEFINE_ADD_VALUE_TEST(TESTCASE, NAME, VALUE, ...) \
TEST(TESTCASE, NAME) \
{ \
    expect_memory_after_add_value(VALUE, __VA_ARGS__); \
}
