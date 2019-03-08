#pragma once

#include <gtest/gtest.h>
#include "cte_encoder.h"
#include "cte_decoder.h"
#include "test_utils.h"

namespace cte_test
{

void expect_encode_produces_status(int buffer_size,
                                   int max_container_depth,
                                   int indent_spaces,
                                   std::shared_ptr<enc::encoding> encoding,
                                   cte_encode_status expected_encode_status);

cte_encode_status expect_encode_produces_data_and_status(int buffer_size,
                                                         int max_container_depth,
                                                         int indent_spaces,
                                                         std::shared_ptr<enc::encoding> encoding,
                                                         const std::string expected_memory,
                                                         cte_encode_status expected_status);


void expect_decode_produces_status(int buffer_size,
                                   int max_container_depth,
                                   bool callback_return_value,
                                   std::string document,
                                   cte_decode_status expected_decode_status);

void expect_decode_produces_data_and_status(int buffer_size,
                                            int max_container_depth,
                                            bool callback_return_value,
                                            const std::string memory,
                                            std::shared_ptr<enc::encoding> expected_encoding,
                                            cte_decode_status expected_status);


void expect_encode_decode_produces_status(int buffer_size,
                                          int max_container_depth,
                                          int indent_spaces,
                                          bool callback_return_value,
                                          std::shared_ptr<enc::encoding> encoding,
                                          cte_encode_status expected_encode_status,
                                          cte_decode_status expected_decode_status);

void expect_encode_decode_produces_data_and_status(int buffer_size,
                                                   int max_container_depth,
                                                   int indent_spaces,
                                                   std::shared_ptr<enc::encoding> expected_encoding,
                                                   const std::string expected_memory,
                                                   cte_encode_status expected_encode_status,
                                                   cte_decode_status expected_decode_status);

void expect_encode_decode_with_shrinking_buffer_size(int min_buffer_size,
                                                     std::shared_ptr<enc::encoding> expected_encoding,
                                                     const std::string expected_memory);
} // namespace cte_test


// Test that the specified encoding produces the expected memory.
#define TEST_ENCODE_DATA(TESTCASE, NAME, BUFFER_SIZE, MAX_DEPTH, INDENT, ENCODING, ...) \
TEST(TESTCASE, NAME) \
{ \
    const std::string memory = __VA_ARGS__; \
    cte_test::expect_encode_produces_data_and_status(BUFFER_SIZE, \
                                                     MAX_DEPTH, \
                                                     INDENT, \
                                                     ENCODING, \
                                                     memory, \
                                                     CTE_ENCODE_STATUS_OK); \
}

// Test that encoding results in the specified status code.
#define TEST_ENCODE_STATUS(TESTCASE, NAME, BUFFER_SIZE, MAX_DEPTH, INDENT, EXPECTED_ENCODE_STATUS, ENCODING) \
TEST(TESTCASE, NAME) \
{ \
    cte_test::expect_encode_produces_status(BUFFER_SIZE, \
                                            MAX_DEPTH, \
                                            INDENT, \
                                            ENCODING, \
                                            EXPECTED_ENCODE_STATUS); \
}

// Test that the specified memory decodes into the equivalent of the expected encoding.
#define TEST_DECODE_DATA(TESTCASE, NAME, BUFFER_SIZE, MAX_DEPTH, ENCODING, ...) \
TEST(TESTCASE, NAME) \
{ \
    const std::string memory = __VA_ARGS__; \
    cte_test::expect_decode_produces_data_and_status(BUFFER_SIZE, \
                                                     MAX_DEPTH, \
                                                     true, \
                                                     memory, \
                                                     ENCODING, \
                                                     CTE_DECODE_STATUS_OK); \
}

// Test that decoding results in the specified status code.
#define TEST_DECODE_STATUS(TESTCASE, NAME, BUFFER_SIZE, MAX_DEPTH, RETURN_VALUE, EXPECTED_DECODE_STATUS, ...) \
TEST(TESTCASE, NAME) \
{ \
    const std::string memory = __VA_ARGS__; \
    cte_test::expect_decode_produces_status(BUFFER_SIZE, \
                                            MAX_DEPTH, \
                                            RETURN_VALUE, \
                                            memory, \
                                            EXPECTED_DECODE_STATUS); \
}

// Test that encoding and decoding results in success.
#define TEST_ENCODE_DECODE_DATA(TESTCASE, NAME, BUFFER_SIZE, MAX_DEPTH, INDENT, ENCODING, ...) \
TEST(TESTCASE, NAME) \
{ \
    const std::string memory = __VA_ARGS__; \
    cte_test::expect_encode_decode_produces_data_and_status(BUFFER_SIZE, \
                                                            MAX_DEPTH, \
                                                            INDENT, \
                                                            ENCODING, \
                                                            memory, \
                                                            CTE_ENCODE_STATUS_OK, \
                                                            CTE_DECODE_STATUS_OK); \
}

// Test that encoding and decoding results in the specified status codes.
#define TEST_ENCODE_DECODE_STATUS(TESTCASE, NAME, BUFFER_SIZE, MAX_DEPTH, INDENT, EXPECTED_ENCODE_STATUS, EXPECTED_DECODE_STATUS, ENCODING) \
TEST(TESTCASE, NAME) \
{ \
    cte_test::expect_encode_decode_produces_status(BUFFER_SIZE, \
                                                   MAX_DEPTH, \
                                                   INDENT, \
                                                   true, \
                                                   ENCODING, \
                                                   EXPECTED_ENCODE_STATUS, \
                                                   EXPECTED_DECODE_STATUS); \
}

// Test that encoding and decoding produces the specified memory, and reproduces the equivalent encoding.
#define TEST_ENCODE_DECODE_SHRINKING(TESTCASE, NAME, MIN_BUFFER_SIZE, ENCODING, ...) \
TEST(TESTCASE, NAME) \
{ \
    cte_test::expect_encode_decode_with_shrinking_buffer_size(MIN_BUFFER_SIZE, ENCODING, __VA_ARGS__); \
}

// Test that decoding results in the specified status code.
#define TEST_STOP_IN_CALLBACK(TESTCASE, NAME, ENCODING) \
TEST(TESTCASE, NAME) \
{ \
    cte_test::expect_encode_decode_produces_status(99, \
                                                   9, \
                                                   0, \
                                                   false, \
                                                   ENCODING, \
                                                   CTE_ENCODE_STATUS_OK, \
                                                   CTE_DECODE_STATUS_STOPPED_IN_CALLBACK); \
}
