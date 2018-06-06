#pragma once

#include "test_helpers.h"
#include "roundtrip_reencode.h"


#define DEFINE_ROUNDTRIP_TEST(TESTCASE, NAME, ...) \
TEST(TESTCASE, NAME) \
{ \
    const std::vector<uint8_t> expected = __VA_ARGS__; \
    expect_memory_after_operation([=](cbe_buffer* buffer) \
    { \
        roundtrip_reencode(expected.data(), expected.size(), buffer); \
        return true; \
    }, \
     __VA_ARGS__); \
}
