#include "test_helpers.h"

template<typename T>
static std::vector<T> make_values_of_length(int length)
{
	std::vector<T> vec;
	for(int i = 0; i < length; i++)
	{
		vec.push_back((T)(i % 128));
	}
	return vec;
}

template<typename T>
static void add_bytes(std::vector<uint8_t>& bytes, T value)
{
	int8_t* ptr = (int8_t*)&value;
	bytes.insert(bytes.end(), ptr, ptr+sizeof(value));
}

template<typename T>
static void test_array_write(int length, std::vector<uint8_t> expected_prefix)
{
	std::vector<T> entities = make_values_of_length<T>(length);
	std::vector<uint8_t> expected_memory(expected_prefix);
	for(auto entity: entities)
	{
		add_bytes(expected_memory, entity);
	}
	expect_memory_after_write(entities, expected_memory);
}

#define DEFINE_ARRAY_WRITE_TEST(NAME, TYPE, LENGTH, ...) \
TEST(NAME, length_ ## LENGTH) \
{ \
    test_array_write<TYPE>(LENGTH, __VA_ARGS__); \
}
#define DEFINE_ARRAY_TEST_SET(NAME, TYPE, CBE_TYPE) \
DEFINE_ARRAY_WRITE_TEST(NAME, TYPE, 0, {TYPE_ARRAY, CBE_TYPE, 0}) \
DEFINE_ARRAY_WRITE_TEST(NAME, TYPE, 1, {TYPE_ARRAY, CBE_TYPE, 1}) \
DEFINE_ARRAY_WRITE_TEST(NAME, TYPE, 2, {TYPE_ARRAY, CBE_TYPE, 2})


// ######################

DEFINE_ARRAY_TEST_SET(ArrayInt16Test, int16_t, TYPE_INT_16)
DEFINE_ARRAY_TEST_SET(ArrayInt32Test, int32_t, TYPE_INT_32)
DEFINE_ARRAY_TEST_SET(ArrayInt64Test, int64_t, TYPE_INT_64)
DEFINE_ARRAY_TEST_SET(ArrayInt128Test, __int128, TYPE_INT_128)
DEFINE_ARRAY_TEST_SET(ArrayFloat32Test, float, TYPE_FLOAT_32)
DEFINE_ARRAY_TEST_SET(ArrayFloat64Test, double, TYPE_FLOAT_64)

// TODO: 128-bit float, and possibly all float, is broken.
// DEFINE_ARRAY_TEST_SET(ArrayFloat128Test, long double, TYPE_FLOAT_128)
