#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <cbe/cbe.h>


#define FILL_CALLBACKS(CALLBACKS_PTR) \
{ \
    (CALLBACKS_PTR)->on_add_empty = on_add_empty; \
    (CALLBACKS_PTR)->on_add_boolean = on_add_boolean; \
    (CALLBACKS_PTR)->on_add_int_8 = on_add_int_8; \
    (CALLBACKS_PTR)->on_add_int_16 = on_add_int_16; \
    (CALLBACKS_PTR)->on_add_int_32 = on_add_int_32; \
    (CALLBACKS_PTR)->on_add_int_64 = on_add_int_64; \
    (CALLBACKS_PTR)->on_add_int_128 = on_add_int_128; \
    (CALLBACKS_PTR)->on_add_float_32 = on_add_float_32; \
    (CALLBACKS_PTR)->on_add_float_64 = on_add_float_64; \
    (CALLBACKS_PTR)->on_add_float_128 = on_add_float_128; \
    (CALLBACKS_PTR)->on_add_decimal_32 = on_add_decimal_32; \
    (CALLBACKS_PTR)->on_add_decimal_64 = on_add_decimal_64; \
    (CALLBACKS_PTR)->on_add_decimal_128 = on_add_decimal_128; \
    (CALLBACKS_PTR)->on_add_time = on_add_time; \
    (CALLBACKS_PTR)->on_end_container = on_end_container; \
    (CALLBACKS_PTR)->on_begin_list = on_begin_list; \
    (CALLBACKS_PTR)->on_begin_map = on_begin_map; \
    (CALLBACKS_PTR)->on_begin_string = on_begin_string; \
    (CALLBACKS_PTR)->on_begin_binary = on_begin_binary; \
    (CALLBACKS_PTR)->on_add_data = on_add_data; \
}

#ifdef __cplusplus
extern "C" {
#endif

const cbe_decode_callbacks* get_always_false_decode_callbacks();
const cbe_decode_callbacks* get_always_true_decode_callbacks();

struct cbe_decode_process* new_always_false_decode_process();
struct cbe_decode_process* new_always_true_decode_process();

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus


#include <functional>
#include <stdio.h>
#include <gtest/gtest.h>
#include <cbe/cbe.h>
#include "decode_encode.h"


#define EXPECT_ENCODE_OK(...) EXPECT_EQ(CBE_ENCODE_STATUS_OK, __VA_ARGS__)

#define EXPECT_DECODE_OK(...) EXPECT_EQ(CBE_DECODE_STATUS_OK, __VA_ARGS__)

void expect_memory_after_operation(
                        std::function<cbe_encode_status(struct cbe_encode_process* encode_process)> operation,
                        std::vector<uint8_t> const& expected_memory);

void expect_incomplete_operation(
                        const int buffer_size,
                        std::function<cbe_encode_status(struct cbe_encode_process* encode_process)> operation);

void expect_incomplete_operation_decrementing(
                        const int buffer_size,
                        std::function<cbe_encode_status(struct cbe_encode_process* encode_process)> operation);

void expect_piecemeal_operation(
                        const int buffer_cutoff,
                        std::function<cbe_encode_status(struct cbe_encode_process* encode_process)> operation1,
                        std::function<cbe_encode_status(struct cbe_encode_process* encode_process)> operation2,
                        std::vector<uint8_t> const& expected_memory);

void expect_piecemeal_operation_decrementing(
                        const int buffer_cutoff_high,
                        const int buffer_cutoff_low,
                        std::function<cbe_encode_status(struct cbe_encode_process* encode_process)> operation1,
                        std::function<cbe_encode_status(struct cbe_encode_process* encode_process)> operation2,
                        std::vector<uint8_t> const& expected_memory);

void expect_decode_encode(std::vector<uint8_t> const& expected_memory);

void buffered_decode_feed(cbe_decode_process* process, const std::vector<uint8_t>& src_document, int buffer_size);

template<typename T> static inline std::vector<T> make_values_of_length(int length)
{
    std::vector<T> vec;
    for(int i = 0; i < length; i++)
    {
        vec.push_back((T)(i & 0x7f));
    }
    return vec;
}


// Internal

template <typename T> static inline cbe_encode_status add_value(struct cbe_encode_process* encode_process, T value)
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
template <> inline cbe_encode_status add_value<SCALAR_TYPE>(struct cbe_encode_process* encode_process, SCALAR_TYPE value) \
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
DEFINE_ADD_VALUE_FUNCTION(__float128,  cbe_encode_add_float_128)
DEFINE_ADD_VALUE_FUNCTION(_Decimal32,  cbe_encode_add_decimal_32)
DEFINE_ADD_VALUE_FUNCTION(_Decimal64,  cbe_encode_add_decimal_64)
DEFINE_ADD_VALUE_FUNCTION(_Decimal128, cbe_encode_add_decimal_128)

template <> inline cbe_encode_status add_value<std::string>(struct cbe_encode_process* encode_process, std::string value)
{
    int64_t length = value.size();
    cbe_encode_status status = cbe_encode_begin_string(encode_process, length);
    if(status != CBE_ENCODE_STATUS_OK)
    {
        return status;
    }
    return cbe_encode_add_data(encode_process, (uint8_t*)value.c_str(), length);
}

template <> inline cbe_encode_status add_value<std::vector<uint8_t>>(struct cbe_encode_process* encode_process, std::vector<uint8_t> value)
{
    int64_t length = value.size();
    cbe_encode_status status = cbe_encode_begin_binary(encode_process, length);
    if(status != CBE_ENCODE_STATUS_OK)
    {
        return status;
    }
    return cbe_encode_add_data(encode_process, value.data(), length);
}

// Test functions

template<typename T> static inline void add_bytes(std::vector<uint8_t>& bytes, T value)
{
    int8_t* ptr = (int8_t*)&value;
    bytes.insert(bytes.end(), ptr, ptr+sizeof(value));
}

template<typename T> static inline void expect_memory_after_add_value(T writeValue, std::vector<uint8_t> const& expected_memory)
{
    expect_memory_after_operation([&](struct cbe_encode_process* encode_process) {return add_value(encode_process, writeValue);}, expected_memory);
}

template<typename T> static inline void expect_add_value_incomplete(const int buffer_size, T writeValue)
{
    expect_incomplete_operation(buffer_size, [&](struct cbe_encode_process* encode_process) \
    { \
        return add_value(encode_process, writeValue); \
    }); \
}

template<typename T> static inline void expect_add_value_incomplete_decrementing(const int buffer_size, T writeValue)
{
    expect_incomplete_operation_decrementing(buffer_size, [&](struct cbe_encode_process* encode_process) \
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

#define DEFINE_ADD_INCOMPLETE_TEST(TESTCASE, NAME, SIZE, VALUE) \
TEST(TESTCASE, NAME) \
{ \
    expect_add_value_incomplete_decrementing(SIZE, VALUE); \
}





// ==================================================================
// ==================================================================


#endif // __cplusplus
