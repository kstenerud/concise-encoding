#pragma once

#include <gtest/gtest.h>
#include "cbe_encoder.h"
#include "cbe_decoder.h"
#include "test_utils.h"

namespace cbe_test
{

void expect_encode_produces_status(int buffer_size,
                                   int max_container_depth,
                                   std::shared_ptr<enc::encoding> encoding,
                                   cbe_encode_status expected_encode_status);

cbe_encode_status expect_encode_produces_data_and_status(int buffer_size,
                                                         int max_container_depth,
                                                         std::shared_ptr<enc::encoding> encoding,
                                                         const std::vector<uint8_t> expected_memory,
                                                         cbe_encode_status expected_status);


void expect_decode_produces_status(int buffer_size,
                                   int max_container_depth,
                                   bool callback_return_value,
                                   std::vector<uint8_t> document,
                                   cbe_decode_status expected_decode_status);

void expect_decode_produces_data_and_status(int buffer_size,
                                            int max_container_depth,
                                            bool callback_return_value,
                                            const std::vector<uint8_t> memory,
                                            std::shared_ptr<enc::encoding> expected_encoding,
                                            cbe_decode_status expected_status);


void expect_encode_decode_produces_status(int buffer_size,
                                          int max_container_depth,
                                          bool callback_return_value,
                                          std::shared_ptr<enc::encoding> encoding,
                                          cbe_encode_status expected_encode_status,
                                          cbe_decode_status expected_decode_status);

void expect_encode_decode_produces_data_and_status(int buffer_size,
                                                   int max_container_depth,
                                                   std::shared_ptr<enc::encoding> expected_encoding,
                                                   const std::vector<uint8_t> expected_memory,
                                                   cbe_encode_status expected_encode_status,
                                                   cbe_decode_status expected_decode_status);

void expect_encode_decode_with_shrinking_buffer_size(int min_buffer_size,
                                                     std::shared_ptr<enc::encoding> expected_encoding,
                                                     const std::vector<uint8_t> expected_memory);
} // namespace cbe_test


// Test that the specified encoding produces the expected memory.
#define TEST_ENCODE_DATA(TESTCASE, NAME, BUFFER_SIZE, MAX_DEPTH, ENCODING, ...) \
TEST(TESTCASE, NAME) \
{ \
    const std::vector<uint8_t> memory = __VA_ARGS__; \
    cbe_test::expect_encode_produces_data_and_status(BUFFER_SIZE, \
                                                     MAX_DEPTH, \
                                                     ENCODING, \
                                                     memory, \
                                                     CBE_ENCODE_STATUS_OK); \
}

// Test that encoding results in the specified status code.
#define TEST_ENCODE_STATUS(TESTCASE, NAME, BUFFER_SIZE, MAX_DEPTH, EXPECTED_ENCODE_STATUS, ENCODING) \
TEST(TESTCASE, NAME) \
{ \
    cbe_test::expect_encode_produces_status(BUFFER_SIZE, \
                                            MAX_DEPTH, \
                                            ENCODING, \
                                            EXPECTED_ENCODE_STATUS); \
}

// Test that the specified memory decodes into the equivalent of the expected encoding.
#define TEST_DECODE_DATA(TESTCASE, NAME, BUFFER_SIZE, MAX_DEPTH, ENCODING, ...) \
TEST(TESTCASE, NAME) \
{ \
    const std::vector<uint8_t> memory = __VA_ARGS__; \
    cbe_test::expect_decode_produces_data_and_status(BUFFER_SIZE, \
                                                     MAX_DEPTH, \
                                                     memory, \
                                                     ENCODING, \
                                                     CBE_DECODE_STATUS_OK); \
}

// Test that decoding results in the specified status code.
#define TEST_DECODE_STATUS(TESTCASE, NAME, BUFFER_SIZE, MAX_DEPTH, RETURN_VALUE, EXPECTED_DECODE_STATUS, ...) \
TEST(TESTCASE, NAME) \
{ \
    const std::vector<uint8_t> memory = __VA_ARGS__; \
    cbe_test::expect_decode_produces_status(BUFFER_SIZE, \
                                            MAX_DEPTH, \
                                            RETURN_VALUE, \
                                            memory, \
                                            EXPECTED_DECODE_STATUS); \
}

// Test that encoding and decoding results in success.
#define TEST_ENCODE_DECODE_DATA(TESTCASE, NAME, BUFFER_SIZE, MAX_DEPTH, ENCODING, ...) \
TEST(TESTCASE, NAME) \
{ \
    const std::vector<uint8_t> memory = __VA_ARGS__; \
    cbe_test::expect_encode_decode_produces_data_and_status(BUFFER_SIZE, \
                                                            MAX_DEPTH, \
                                                            ENCODING, \
                                                            memory, \
                                                            CBE_ENCODE_STATUS_OK, \
                                                            CBE_DECODE_STATUS_OK); \
}

// Test that encoding and decoding results in the specified status codes.
#define TEST_ENCODE_DECODE_STATUS(TESTCASE, NAME, BUFFER_SIZE, MAX_DEPTH, EXPECTED_ENCODE_STATUS, EXPECTED_DECODE_STATUS, ENCODING) \
TEST(TESTCASE, NAME) \
{ \
    cbe_test::expect_encode_decode_produces_status(BUFFER_SIZE, \
                                                   MAX_DEPTH, \
                                                   true, \
                                                   ENCODING, \
                                                   EXPECTED_ENCODE_STATUS, \
                                                   EXPECTED_DECODE_STATUS); \
}

// Test that encoding and decoding produces the specified memory, and reproduces the equivalent encoding.
#define TEST_ENCODE_DECODE_SHRINKING(TESTCASE, NAME, MIN_BUFFER_SIZE, ENCODING, ...) \
TEST(TESTCASE, NAME) \
{ \
    cbe_test::expect_encode_decode_with_shrinking_buffer_size(MIN_BUFFER_SIZE, ENCODING, __VA_ARGS__); \
}

// Test that decoding results in the specified status code.
#define TEST_STOP_IN_CALLBACK(TESTCASE, NAME, ENCODING) \
TEST(TESTCASE, NAME) \
{ \
    cbe_test::expect_encode_decode_produces_status(99, \
                                                   9, \
                                                   false, \
                                                   ENCODING, \
                                                   CBE_ENCODE_STATUS_OK, \
                                                   CBE_DECODE_STATUS_STOPPED_IN_CALLBACK); \
}
