#include "test_helpers.h"

template<typename T>
static void expect_encode_decode_encode_array(int length, std::vector<uint8_t> expected_prefix)
{
    std::vector<T> entities = make_values_of_length<T>(length);
    std::vector<uint8_t> expected_memory(expected_prefix);
    for(auto entity: entities)
    {
        add_bytes(expected_memory, entity);
    }
    expect_memory_after_add_value(entities, expected_memory);
    expect_decode_encode(expected_memory);
}

template<typename T>
static void expect_failed_add_array(int byte_length, int array_length)
{
    std::vector<T> entities = make_values_of_length<T>(array_length);
    expect_add_value_fail(byte_length, entities);
}

#define DEFINE_ARRAY_TEST(NAME, TYPE, LENGTH, ...) \
TEST(NAME, length_ ## LENGTH) \
{ \
    expect_encode_decode_encode_array<TYPE>(LENGTH, __VA_ARGS__); \
}
#define DEFINE_ARRAY_TEST_SET(NAME, TYPE, CBE_TYPE) \
DEFINE_ARRAY_TEST(NAME, TYPE, 0, {CBE_TYPE, 0}) \
DEFINE_ARRAY_TEST(NAME, TYPE, 1, {CBE_TYPE, 4}) \
DEFINE_ARRAY_TEST(NAME, TYPE, 2, {CBE_TYPE, 8})

#define DEFINE_FAILED_ARRAY_TEST(TYPE) \
TEST(ArrayFail, type_ ## TYPE) \
{ \
    int array_length = 10; \
    int byte_length = array_length * sizeof(TYPE) - 1; \
    expect_failed_add_array<TYPE>(byte_length, array_length); \
}

// ######################

DEFINE_ARRAY_TEST_SET(ArrayInt8Test, int8_t, TYPE_ARRAY_INT_8)
DEFINE_ARRAY_TEST_SET(ArrayInt16Test, int16_t, TYPE_ARRAY_INT_16)
DEFINE_ARRAY_TEST_SET(ArrayInt32Test, int32_t, TYPE_ARRAY_INT_32)
DEFINE_ARRAY_TEST_SET(ArrayInt64Test, int64_t, TYPE_ARRAY_INT_64)
DEFINE_ARRAY_TEST_SET(ArrayInt128Test, __int128, TYPE_ARRAY_INT_128)
DEFINE_ARRAY_TEST_SET(ArrayFloat32Test, float, TYPE_ARRAY_FLOAT_32)
DEFINE_ARRAY_TEST_SET(ArrayFloat64Test, double, TYPE_ARRAY_FLOAT_64)
// DEFINE_ARRAY_TEST_SET(ArrayFloat128Test, long double, TYPE_FLOAT_128)

static void expect_memory_after_add_array_boolean(std::vector<bool> entities, std::vector<uint8_t> expected_memory)
{
    expect_memory_after_add_value(entities, expected_memory);
}

#define DEFINE_BOOLEAN_ARRAY_ADD_TEST(NAME, ...) \
TEST(BooleanArrayTest, NAME) \
{ \
    expect_memory_after_add_array_boolean(__VA_ARGS__); \
}

DEFINE_BOOLEAN_ARRAY_ADD_TEST(size_0, {}, {TYPE_ARRAY_BOOLEAN, 0x00})
DEFINE_BOOLEAN_ARRAY_ADD_TEST(size_1, {true}, {TYPE_ARRAY_BOOLEAN, 0x04, 0x01})
DEFINE_BOOLEAN_ARRAY_ADD_TEST(size_2, {true, true}, {TYPE_ARRAY_BOOLEAN, 0x08, 0x03})
DEFINE_BOOLEAN_ARRAY_ADD_TEST(size_3, {true, false, true}, {TYPE_ARRAY_BOOLEAN, 0x0c, 0x05})
DEFINE_BOOLEAN_ARRAY_ADD_TEST(size_9, {true, false, true, false, false, true, false, true, true}, {TYPE_ARRAY_BOOLEAN, 0x24, 0xa5, 0x01})

DEFINE_FAILED_ARRAY_TEST(int8_t)
DEFINE_FAILED_ARRAY_TEST(int16_t)
DEFINE_FAILED_ARRAY_TEST(int32_t)
DEFINE_FAILED_ARRAY_TEST(int64_t)
DEFINE_FAILED_ARRAY_TEST(__int128)
DEFINE_FAILED_ARRAY_TEST(float)
DEFINE_FAILED_ARRAY_TEST(double)
