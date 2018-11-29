#include "test_helpers.h"

TEST(DecodeInvalid, ListNoEnd)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_list(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end(encode_process));
}

TEST(DecodeInvalid, MapNoEnd)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end(encode_process));
}

TEST(DecodeInvalid, ListMapNoEnd)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_list(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end(encode_process));
}

TEST(DecodeInvalid, ListMapNoEnd2)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_list(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_end_container(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end(encode_process));
}

TEST(DecodeInvalid, ListTooManyEnd)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_list(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_end_container(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end_container(encode_process));
	cbe_encode_end(encode_process);
}

TEST(DecodeInvalid, MapTooManyEnd)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_end_container(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end_container(encode_process));
	cbe_encode_end(encode_process);
}

TEST(DecodeInvalid, ListMapTooManyEnd)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_list(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_end_container(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_end_container(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end_container(encode_process));
	cbe_encode_end(encode_process);
}

TEST(DecodeInvalid, UnclosedList)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_list(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end(encode_process));
}

TEST(DecodeInvalid, UnclosedMap)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end(encode_process));
}

TEST(DecodeInvalid, EmptyKey)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_INCORRECT_KEY_TYPE, cbe_encode_add_empty(encode_process));
}

TEST(DecodeInvalid, ListKey)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_INCORRECT_KEY_TYPE, cbe_encode_begin_list(encode_process));
}

TEST(DecodeInvalid, MapKey)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_INCORRECT_KEY_TYPE, cbe_encode_begin_map(encode_process));
}

TEST(DecodeInvalid, BitfieldKey)
{
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;
    uint8_t bitfield[1];

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_INCORRECT_KEY_TYPE, cbe_encode_add_bitfield(encode_process, bitfield, 4));
}

#define DEFINE_ARRAY_KEY_TEST(TYPE, FUNCTION) \
TEST(DecodeInvalid, ArrayKey_ ## TYPE) \
{ \
    const int memory_size = 100000; \
    std::array<uint8_t, memory_size> memory; \
    int element_count = 10; \
    TYPE array[element_count]; \
	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size()); \
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process)); \
	EXPECT_EQ(CBE_ENCODE_STATUS_INCORRECT_KEY_TYPE, FUNCTION(encode_process, array, element_count)); \
}
DEFINE_ARRAY_KEY_TEST(bool,        cbe_encode_add_array_boolean)
DEFINE_ARRAY_KEY_TEST(int8_t,      cbe_encode_add_array_int_8)
DEFINE_ARRAY_KEY_TEST(int16_t,     cbe_encode_add_array_int_16)
DEFINE_ARRAY_KEY_TEST(int32_t,     cbe_encode_add_array_int_32)
DEFINE_ARRAY_KEY_TEST(int64_t,     cbe_encode_add_array_int_64)
DEFINE_ARRAY_KEY_TEST(__int128,    cbe_encode_add_array_int_128)
DEFINE_ARRAY_KEY_TEST(float,       cbe_encode_add_array_float_32)
DEFINE_ARRAY_KEY_TEST(double,      cbe_encode_add_array_float_64)
DEFINE_ARRAY_KEY_TEST(__float128,  cbe_encode_add_array_float_128)
DEFINE_ARRAY_KEY_TEST(_Decimal32,  cbe_encode_add_array_decimal_32)
DEFINE_ARRAY_KEY_TEST(_Decimal64,  cbe_encode_add_array_decimal_64)
DEFINE_ARRAY_KEY_TEST(_Decimal128, cbe_encode_add_array_decimal_128)
DEFINE_ARRAY_KEY_TEST(smalltime,   cbe_encode_add_array_time)

#define DEFINE_MISSING_MAP_VALUE_TEST(NAME, TYPE, VALUE) \
TEST(DecodeInvalid, MissingMapValue_ ## NAME) \
{ \
    const int memory_size = 100000; \
    std::array<uint8_t, memory_size> memory; \
	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size()); \
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process)); \
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_add_ ## TYPE(encode_process, VALUE)); \
	EXPECT_EQ(CBE_ENCODE_STATUS_MISSING_VALUE_FOR_KEY, cbe_encode_end_container(encode_process)); \
	cbe_encode_end(encode_process); \
}
DEFINE_MISSING_MAP_VALUE_TEST(int8, int_8, 1)
DEFINE_MISSING_MAP_VALUE_TEST(int16, int_16, 1)
DEFINE_MISSING_MAP_VALUE_TEST(int32, int_32, 1)
DEFINE_MISSING_MAP_VALUE_TEST(int64, int_64, 1)
DEFINE_MISSING_MAP_VALUE_TEST(int128, int_128, 1)
DEFINE_MISSING_MAP_VALUE_TEST(float32, float_32, 1)
DEFINE_MISSING_MAP_VALUE_TEST(float64, float_64, 1)
DEFINE_MISSING_MAP_VALUE_TEST(float128, float_128, 1)
DEFINE_MISSING_MAP_VALUE_TEST(decimal32, decimal_32, 1)
DEFINE_MISSING_MAP_VALUE_TEST(decimal64, decimal_64, 1)
DEFINE_MISSING_MAP_VALUE_TEST(decimal128, decimal_128, 1)
DEFINE_MISSING_MAP_VALUE_TEST(time, time, 1)
DEFINE_MISSING_MAP_VALUE_TEST(string, string, "1")
DEFINE_MISSING_MAP_VALUE_TEST(long_string, string, "this is a longer string for testing")

TEST(DecodeInvalid, MissingMapValue_Depth) \
{ \
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;
	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process));
		EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_add_int_8(encode_process, 1));
		EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process));
			EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_add_int_8(encode_process, 2));
			EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_add_int_8(encode_process, 3));
		EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_end_container(encode_process));
		EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_add_int_8(encode_process, 4));
	EXPECT_EQ(CBE_ENCODE_STATUS_MISSING_VALUE_FOR_KEY, cbe_encode_end_container(encode_process));
	cbe_encode_end(encode_process);
}

TEST(DecodeInvalid, ContainersTooDeep)
{
	const int depth_too_far = 10000;
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;
    uint8_t bitfield[1];

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	cbe_encode_status status;
	for(int i = 0; i < depth_too_far; i++)
	{
		status = cbe_encode_begin_list(encode_process);
		if(status == CBE_ENCODE_STATUS_MAX_CONTAINER_DEPTH_EXCEEDED)
		{
			break;
		}
	}
	EXPECT_EQ(CBE_ENCODE_STATUS_MAX_CONTAINER_DEPTH_EXCEEDED, status);
}

