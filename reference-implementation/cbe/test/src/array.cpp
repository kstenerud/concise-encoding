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
static void expect_add_array_incomplete(int byte_length, int array_length)
{
    std::vector<T> entities = make_values_of_length<T>(array_length);
    expect_add_value_incomplete(byte_length, entities);
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

#define DEFINE_ARRAY_INCOMPLETE_TEST(TYPE) \
TEST(ArrayIncomplete, type_ ## TYPE) \
{ \
    int array_length = 10; \
    int byte_length = array_length * sizeof(TYPE) - 1; \
    expect_add_array_incomplete<TYPE>(byte_length, array_length); \
}

// ######################

DEFINE_ARRAY_TEST_SET(ArrayInt8Test, int8_t, 0x74)
DEFINE_ARRAY_TEST_SET(ArrayInt16Test, int16_t, 0x75)
DEFINE_ARRAY_TEST_SET(ArrayInt32Test, int32_t, 0x76)
DEFINE_ARRAY_TEST_SET(ArrayInt64Test, int64_t, 0x77)
DEFINE_ARRAY_TEST_SET(ArrayInt128Test, __int128, 0x78)
DEFINE_ARRAY_TEST_SET(ArrayFloat32Test, float, 0x79)
DEFINE_ARRAY_TEST_SET(ArrayFloat64Test, double, 0x7a)
DEFINE_ARRAY_TEST_SET(ArrayFloat128Test, __float128, 0x7b)
DEFINE_ARRAY_TEST_SET(ArrayDecimal32Test, _Decimal32, 0x7c)
DEFINE_ARRAY_TEST_SET(ArrayDecimal64Test, _Decimal64, 0x7d)
DEFINE_ARRAY_TEST_SET(ArrayDecimal128Test, _Decimal128, 0x7e)
// TODO: This won't work because typedef doesn't make a real type :/
// SEE: test_helpers.h DEFINE_ADD_VALUE_FUNCTION for time
// DEFINE_ARRAY_TEST_SET(ArrayTimeTest, smalltime, 0x7f)

static void expect_memory_after_add_array_boolean(std::vector<bool> entities, std::vector<uint8_t> expected_memory)
{
    expect_memory_after_add_value(entities, expected_memory);
}

#define DEFINE_BOOLEAN_ARRAY_ADD_TEST(NAME, ...) \
TEST(BooleanArrayTest, NAME) \
{ \
    expect_memory_after_add_array_boolean(__VA_ARGS__); \
}

DEFINE_BOOLEAN_ARRAY_ADD_TEST(size_0, {}, {0x73, 0x00})
DEFINE_BOOLEAN_ARRAY_ADD_TEST(size_1, {true}, {0x73, 0x04, 0x01})
DEFINE_BOOLEAN_ARRAY_ADD_TEST(size_2, {true, true}, {0x73, 0x08, 0x03})
DEFINE_BOOLEAN_ARRAY_ADD_TEST(size_3, {true, false, true}, {0x73, 0x0c, 0x05})
DEFINE_BOOLEAN_ARRAY_ADD_TEST(size_9, {true, false, true, false, false, true, false, true, true}, {0x73, 0x24, 0xa5, 0x01})

DEFINE_ARRAY_INCOMPLETE_TEST(int8_t)
DEFINE_ARRAY_INCOMPLETE_TEST(int16_t)
DEFINE_ARRAY_INCOMPLETE_TEST(int32_t)
DEFINE_ARRAY_INCOMPLETE_TEST(int64_t)
DEFINE_ARRAY_INCOMPLETE_TEST(__int128)
DEFINE_ARRAY_INCOMPLETE_TEST(float)
DEFINE_ARRAY_INCOMPLETE_TEST(double)
DEFINE_ARRAY_INCOMPLETE_TEST(__float128)
DEFINE_ARRAY_INCOMPLETE_TEST(_Decimal32)
DEFINE_ARRAY_INCOMPLETE_TEST(_Decimal64)
DEFINE_ARRAY_INCOMPLETE_TEST(_Decimal128)
