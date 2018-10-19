#include "test_helpers.h"


inline void expect_memory_after_add_time(const cbe_time* const time, std::vector<uint8_t> const& expected_memory)
{
    expect_memory_after_operation([&](cbe_buffer* buffer)
    {
        return cbe_add_time(buffer, time);
    }, expected_memory);
}

#define DEFINE_ADD_TIME_TEST(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, USEC, ...) \
TEST(TimeTest, time_ ## YEAR ## _ ## MONTH ## _ ## DAY ## _ ## HOUR ## _ ## MINUTE ## _ ## SECOND ## _ ## USEC) \
{ \
    cbe_time time = \
    { \
        .year = YEAR, \
        .month = MONTH, \
        .day = DAY, \
        .hour = HOUR, \
        .minute = MINUTE, \
        .second = SECOND, \
        .microsecond = USEC \
    }; \
    std::vector<uint8_t> expected_memory = __VA_ARGS__; \
    expect_memory_after_add_time(&time, expected_memory); \
    expect_decode_encode(expected_memory); \
}

#define DEFINE_FAILED_ADD_TIME_TEST(SIZE, YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, USEC) \
TEST(TimeTest, failed_time_ ## YEAR ## _ ## MONTH ## _ ## DAY ## _ ## HOUR ## _ ## MINUTE ## _ ## SECOND ## _ ## USEC) \
{ \
    cbe_time time = \
    { \
        .year = YEAR, \
        .month = MONTH, \
        .day = DAY, \
        .hour = HOUR, \
        .minute = MINUTE, \
        .second = SECOND, \
        .microsecond = USEC \
    }; \
    expect_failed_operation_decrementing(SIZE, [&](cbe_buffer* buffer) \
    { \
        return cbe_add_time(buffer, &time); \
    }); \
}

DEFINE_ADD_TIME_TEST(1955, 11, 5, 8, 21, 0, 0, {0x6a, 0x40, 0x85, 0xca, 0x8e, 0x1e})
DEFINE_ADD_TIME_TEST(2015, 10, 21, 14, 28, 9, 714000, {0x6b, 0x10, 0xe5, 0x9a, 0x70, 0xae, 0xea, 0xf7, 0x01})
DEFINE_ADD_TIME_TEST(1985, 10, 26, 8, 22, 16, 900142, {0x6b, 0x2e, 0xbc, 0x0d, 0x59, 0x48, 0x6b, 0xf0, 0x01})

DEFINE_FAILED_ADD_TIME_TEST(4, 1955, 11, 5, 8, 21, 0, 0)
DEFINE_FAILED_ADD_TIME_TEST(5, 2015, 10, 21, 14, 28, 9, 714000)
DEFINE_FAILED_ADD_TIME_TEST(7, 1985, 10, 26, 8, 22, 16, 900142)
