#include "test_helpers.h"

TEST(DecodeInvalid, ListNoEnd)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	cbe_encode_begin_list(encode_process);
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end(encode_process));
}

TEST(DecodeInvalid, MapNoEnd)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	cbe_encode_begin_map(encode_process);
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end(encode_process));
}

TEST(DecodeInvalid, ListMapNoEnd)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	cbe_encode_begin_list(encode_process);
	cbe_encode_begin_map(encode_process);
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end(encode_process));
}

TEST(DecodeInvalid, ListMapNoEnd2)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	cbe_encode_begin_list(encode_process);
	cbe_encode_begin_map(encode_process);
	cbe_encode_end_container(encode_process);
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end(encode_process));
}

TEST(DecodeInvalid, ListTooManyEnd)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	cbe_encode_begin_list(encode_process);
	cbe_encode_end_container(encode_process);
	cbe_encode_end_container(encode_process);
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end(encode_process));
}

TEST(DecodeInvalid, MapTooManyEnd)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	cbe_encode_begin_map(encode_process);
	cbe_encode_end_container(encode_process);
	cbe_encode_end_container(encode_process);
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end(encode_process));
}

TEST(DecodeInvalid, ListMapTooManyEnd)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	cbe_encode_begin_list(encode_process);
	cbe_encode_begin_map(encode_process);
	cbe_encode_end_container(encode_process);
	cbe_encode_end_container(encode_process);
	cbe_encode_end_container(encode_process);
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end(encode_process));
}
