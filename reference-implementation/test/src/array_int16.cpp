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

#define DEFINE_ARRAY_WRITE_TEST(LENGTH, ...) \
TEST(ArrayInt16Test, length_ ## LENGTH) \
{ \
    test_array_write<int16_t>(LENGTH, __VA_ARGS__); \
}

DEFINE_ARRAY_WRITE_TEST( 0, {TYPE_ARRAY, TYPE_INT_16, 0});
DEFINE_ARRAY_WRITE_TEST( 1, {TYPE_ARRAY, TYPE_INT_16, 1});
DEFINE_ARRAY_WRITE_TEST( 2, {TYPE_ARRAY, TYPE_INT_16, 2});
DEFINE_ARRAY_WRITE_TEST( 252, {TYPE_ARRAY, TYPE_INT_16, 252});
DEFINE_ARRAY_WRITE_TEST( 253, {TYPE_ARRAY, TYPE_INT_16, LENGTH_16BIT, 0xfd, 0x00});
DEFINE_ARRAY_WRITE_TEST( 254, {TYPE_ARRAY, TYPE_INT_16, LENGTH_16BIT, 0xfe, 0x00});
DEFINE_ARRAY_WRITE_TEST( 255, {TYPE_ARRAY, TYPE_INT_16, LENGTH_16BIT, 0xff, 0x00});
DEFINE_ARRAY_WRITE_TEST( 256, {TYPE_ARRAY, TYPE_INT_16, LENGTH_16BIT, 0x00, 0x01});
