#pragma once

#include <gtest/gtest.h>
#include "cbe_encoder.h"
#include "cbe_decoder.h"
#include "as_string.h"

namespace cbe_test
{

std::vector<uint8_t> encode_data(int buffer_size, std::shared_ptr<enc::encoding> encoding);

std::shared_ptr<enc::encoding> decode_data(int buffer_size, std::vector<uint8_t> data);

void expect_encode(int buffer_size, std::shared_ptr<enc::encoding> encoding, const std::vector<uint8_t> expected_memory);

void expect_decode(int buffer_size, const std::vector<uint8_t> memory, std::shared_ptr<enc::encoding> expected_encoding);

void expect_decode_exact(int buffer_size, const std::vector<uint8_t> memory, std::shared_ptr<enc::encoding> expected_encoding);

void expect_encode_decode_equality(int buffer_size, std::shared_ptr<enc::encoding> expected_encoding, const std::vector<uint8_t> expected_memory);

void expect_encode_decode_exact_equality(int buffer_size, std::shared_ptr<enc::encoding> expected_encoding, const std::vector<uint8_t> expected_memory);

}

// Test that the specified encoding produces the expected memory.
#define TEST_ENCODE(TESTCASE, NAME, BUFFER_SIZE, ENCODING, ...) \
TEST(TESTCASE, NAME) {cbe_test::expect_encode(BUFFER_SIZE, ENCODING, __VA_ARGS__);}

// Test that the specified memory decodes into the equivalent of the expected encoding.
#define TEST_DECODE(TESTCASE, NAME, BUFFER_SIZE, ENCODING, ...) \
TEST(TESTCASE, NAME) {std::vector<uint8_t> memory = __VA_ARGS__; cbe_test::expect_decode(BUFFER_SIZE, memory, ENCODING);}

// Test that the specified memory decodes into exactly the expected encoding.
#define TEST_DECODE_EXACT(TESTCASE, NAME, BUFFER_SIZE, ENCODING, ...) \
TEST(TESTCASE, NAME) {std::vector<uint8_t> memory = __VA_ARGS__; cbe_test::expect_decode_exact(BUFFER_SIZE, memory, ENCODING);}

// Test that encoding and decoding produces the specified memory, and reproduces the equivalent encoding.
#define TEST_ENCODE_DECODE(TESTCASE, NAME, BUFFER_SIZE, ENCODING, ...) \
TEST(TESTCASE, NAME) {cbe_test::expect_encode_decode_equality(BUFFER_SIZE, ENCODING, __VA_ARGS__);}

// Test that encoding and decoding produces the specified memory, and reproduces the exact encoding.
#define TEST_ENCODE_DECODE_EXACT(TESTCASE, NAME, BUFFER_SIZE, ENCODING, ...) \
TEST(TESTCASE, NAME) {cbe_test::expect_encode_decode_exact_equality(BUFFER_SIZE, ENCODING, __VA_ARGS__);}
