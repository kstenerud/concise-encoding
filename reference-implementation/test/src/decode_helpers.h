#pragma once

#include "test_helpers.h"
#include "decode_encode.h"


#define DEFINE_ROUNDTRIP_TEST(TESTCASE, NAME, ...) \
TEST(TESTCASE, NAME) \
{ \
    const std::vector<uint8_t> expected = __VA_ARGS__; \
    expect_memory_after_operation([=](cbe_buffer* buffer) \
    { \
        decode_encode(expected.data(), expected.size(), buffer); \
        return true; \
    }, \
     __VA_ARGS__); \
}
