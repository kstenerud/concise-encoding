#include "encode_test_helpers.h"


inline void expect_memory_after_add_date(unsigned year,
										 unsigned month,
										 unsigned day,
										 unsigned hour,
										 unsigned minute,
										 unsigned second,
										 std::vector<uint8_t> const& expected_memory)
{
	expect_memory_after_add_function([&](cbe_buffer* buffer) {return cbe_add_date(buffer, year, month, day, hour, minute, second);}, expected_memory);
}

inline void expect_memory_after_add_timestamp_ms(unsigned year,
												 unsigned month,
												 unsigned day,
												 unsigned hour,
												 unsigned minute,
												 unsigned second,
												 unsigned msec,
												 std::vector<uint8_t> const& expected_memory)
{
	expect_memory_after_add_function([&](cbe_buffer* buffer) {return cbe_add_timestamp(buffer, year, month, day, hour, minute, second, msec);}, expected_memory);
}

inline void expect_memory_after_add_timestamp_ns(unsigned year,
												 unsigned month,
												 unsigned day,
												 unsigned hour,
												 unsigned minute,
												 unsigned second,
												 unsigned nsec,
												 std::vector<uint8_t> const& expected_memory)
{
	expect_memory_after_add_function([&](cbe_buffer* buffer) {return cbe_add_timestamp_ns(buffer, year, month, day, hour, minute, second, nsec);}, expected_memory);
}

#define DEFINE_ADD_DATE_TEST(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, ...) \
TEST(DateTest, date_ ## YEAR ## _ ## MONTH ## _ ## DAY ## _ ## HOUR ## _ ## MINUTE ## _ ## SECOND) \
{ \
	expect_memory_after_add_date(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, __VA_ARGS__); \
}

#define DEFINE_ADD_TIMESTAMP_MS_TEST(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, MSEC, ...) \
TEST(DateTest, timestamp_ms_ ## YEAR ## _ ## MONTH ## _ ## DAY ## _ ## HOUR ## _ ## MINUTE ## _ ## SECOND ## _ ## MSEC) \
{ \
	expect_memory_after_add_timestamp_ms(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, MSEC, __VA_ARGS__); \
}

#define DEFINE_ADD_TIMESTAMP_NS_TEST(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, NSEC, ...) \
TEST(DateTest, timestamp_ns_ ## YEAR ## _ ## MONTH ## _ ## DAY ## _ ## HOUR ## _ ## MINUTE ## _ ## SECOND ## _ ## NSEC) \
{ \
	expect_memory_after_add_timestamp_ns(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, NSEC, __VA_ARGS__); \
}

DEFINE_ADD_DATE_TEST(2000, 1, 1, 0, 0, 0, {TYPE_DATE, 0x00, 0x08, 0x57, 0x37, 0x0f})
DEFINE_ADD_TIMESTAMP_MS_TEST(2000, 1, 1, 0, 0, 0, 0, {TYPE_TIMESTAMP_MS, 0x00, 0x40, 0xf7, 0x2b, 0x70, 0x3b})
DEFINE_ADD_TIMESTAMP_NS_TEST(2000, 1, 1, 0, 0, 0, 0, {TYPE_TIMESTAMP_NS, 0x00, 0x00, 0xd2, 0xbd, 0x2b, 0x2e, 0xe8, 0x00})
