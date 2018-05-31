#include "encode_test_helpers.h"


inline void expect_date_write(unsigned year,
							unsigned month,
							unsigned day,
							unsigned hour,
							unsigned minute,
							unsigned second,
							std::vector<uint8_t>& expected_memory)
{
	int expected_size = expected_memory.size();
	const int memory_size = 100;
	std::array<uint8_t, memory_size> memory;
	uint8_t* data = memory.data();
    uint8_t* expected_pos = data + expected_size;

    cbe_buffer buffer = create_buffer(data, memory_size);

	bool success = cbe_add_date(&buffer, year, month, day, hour, minute, second);
    fflush(stdout);

    std::vector<uint8_t> actual_memory = std::vector<uint8_t>(data, data + expected_size);
    EXPECT_TRUE(success);
    EXPECT_EQ(expected_pos, buffer.pos);
    EXPECT_EQ(expected_memory, actual_memory);
}

inline void expect_timestamp_ms_write(unsigned year,
									unsigned month,
									unsigned day,
									unsigned hour,
									unsigned minute,
									unsigned second,
									unsigned msec,
									std::vector<uint8_t>& expected_memory)
{
	int expected_size = expected_memory.size();
	const int memory_size = 100;
	std::array<uint8_t, memory_size> memory;
	uint8_t* data = memory.data();
    uint8_t* expected_pos = data + expected_size;

    cbe_buffer buffer = create_buffer(data, memory_size);

	bool success = cbe_add_timestamp(&buffer, year, month, day, hour, minute, second, msec);
    fflush(stdout);

    std::vector<uint8_t> actual_memory = std::vector<uint8_t>(data, data + expected_size);
    EXPECT_TRUE(success);
    EXPECT_EQ(expected_pos, buffer.pos);
    EXPECT_EQ(expected_memory, actual_memory);
}

inline void expect_timestamp_ns_write(unsigned year,
									unsigned month,
									unsigned day,
									unsigned hour,
									unsigned minute,
									unsigned second,
									unsigned nsec,
									std::vector<uint8_t>& expected_memory)
{
	int expected_size = expected_memory.size();
	const int memory_size = 100;
	std::array<uint8_t, memory_size> memory;
	uint8_t* data = memory.data();
    uint8_t* expected_pos = data + expected_size;

    cbe_buffer buffer = create_buffer(data, memory_size);

	bool success = cbe_add_timestamp_ns(&buffer, year, month, day, hour, minute, second, nsec);
    fflush(stdout);

    std::vector<uint8_t> actual_memory = std::vector<uint8_t>(data, data + expected_size);
    EXPECT_TRUE(success);
    EXPECT_EQ(expected_pos, buffer.pos);
    EXPECT_EQ(expected_memory, actual_memory);
}

TEST(DateTest, date_1)
{
	uint8_t expected[] = {TYPE_DATE, 0x00, 0x08, 0x57, 0x37, 0x0f};
	std::vector<uint8_t> vec(expected, expected + sizeof(expected) / sizeof(expected[0]));
	expect_date_write(2000, 1, 1, 0, 0, 0, vec);
}

TEST(DateTest, timestamp_msec_1)
{
	uint8_t expected[] = {TYPE_TIMESTAMP_MS, 0x00, 0x40, 0xf7, 0x2b, 0x70, 0x3b};
	std::vector<uint8_t> vec(expected, expected + sizeof(expected) / sizeof(expected[0]));
	expect_timestamp_ms_write(2000, 1, 1, 0, 0, 0, 0, vec);
}

TEST(DateTest, timestamp_nsec_1)
{
	uint8_t expected[] = {TYPE_TIMESTAMP_NS, 0x00, 0x00, 0xd2, 0xbd, 0x2b, 0x2e, 0xe8, 0x00};
	std::vector<uint8_t> vec(expected, expected + sizeof(expected) / sizeof(expected[0]));
	expect_timestamp_ns_write(2000, 1, 1, 0, 0, 0, 0, vec);
}
