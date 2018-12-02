#include "test_helpers.h"
#include "cbe_internal.h"

DEFINE_DECODE_CALLBACK_SUITE(true);

static cbe_decode_process* new_do_nothing_decode_process(cbe_decode_callbacks* callbacks)
{
    callbacks->on_empty = on_empty;
    callbacks->on_boolean = on_boolean;
    callbacks->on_int_8 = on_int_8;
    callbacks->on_int_16 = on_int_16;
    callbacks->on_int_32 = on_int_32;
    callbacks->on_int_64 = on_int_64;
    callbacks->on_int_128 = on_int_128;
    callbacks->on_float_32 = on_float_32;
    callbacks->on_float_64 = on_float_64;
    callbacks->on_float_128 = on_float_128;
    callbacks->on_decimal_32 = on_decimal_32;
    callbacks->on_decimal_64 = on_decimal_64;
    callbacks->on_decimal_128 = on_decimal_128;
    callbacks->on_time = on_time;
    callbacks->on_end_container = on_end_container;
    callbacks->on_begin_list = on_begin_list;
    callbacks->on_begin_map = on_begin_map;
    callbacks->on_string = on_string;
    callbacks->on_binary_data = on_binary_data;
    return cbe_decode_begin(callbacks, NULL);
}


static cbe_decode_status try_decode(const uint8_t* memory, int64_t length)
{
	cbe_decode_callbacks callbacks;
	cbe_decode_process* decode_process = new_do_nothing_decode_process(&callbacks);
	cbe_decode_status status_feed = cbe_decode_feed(decode_process, memory, length);
	cbe_decode_status status_end = cbe_decode_end(decode_process);
	return status_feed != CBE_DECODE_STATUS_OK ? status_feed : status_end;
}

TEST(DecodeInvalid, ListNoEnd)
{
    const int memory_size = 1000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_list(encode_process));
	int length = cbe_encode_get_buffer_offset(encode_process);
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end(encode_process));
	EXPECT_EQ(CBE_DECODE_STATUS_UNBALANCED_CONTAINERS, try_decode(memory.data(), length));
}

TEST(DecodeInvalid, MapNoEnd)
{
    const int memory_size = 1000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process));
	int length = cbe_encode_get_buffer_offset(encode_process);
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end(encode_process));
	EXPECT_EQ(CBE_DECODE_STATUS_UNBALANCED_CONTAINERS, try_decode(memory.data(), length));
}

TEST(DecodeInvalid, ListMapNoEnd)
{
    const int memory_size = 1000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_list(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process));
	int length = cbe_encode_get_buffer_offset(encode_process);
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end(encode_process));
	EXPECT_EQ(CBE_DECODE_STATUS_UNBALANCED_CONTAINERS, try_decode(memory.data(), length));
}

TEST(DecodeInvalid, ListMapNoEnd2)
{
    const int memory_size = 1000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_list(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_end_container(encode_process));
	int length = cbe_encode_get_buffer_offset(encode_process);
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end(encode_process));
	EXPECT_EQ(CBE_DECODE_STATUS_UNBALANCED_CONTAINERS, try_decode(memory.data(), length));
}

TEST(DecodeInvalid, ListTooManyEnd)
{
    const int memory_size = 1000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_list(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_end_container(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end_container(encode_process));
	cbe_encode_end(encode_process);
}

TEST(DecodeInvalid, MapTooManyEnd)
{
    const int memory_size = 1000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_end_container(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end_container(encode_process));
	cbe_encode_end(encode_process);
	// Cannot test decode for this because reaching container level 0 terminates the document.
}

TEST(DecodeInvalid, ListMapTooManyEnd)
{
    const int memory_size = 1000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_list(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_end_container(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_end_container(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end_container(encode_process));
	cbe_encode_end(encode_process);
	// Cannot test decode for this because reaching container level 0 terminates the document.
}

TEST(DecodeInvalid, UnclosedList)
{
    const int memory_size = 1000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_list(encode_process));
	int length = cbe_encode_get_buffer_offset(encode_process);
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end(encode_process));
	EXPECT_EQ(CBE_DECODE_STATUS_UNBALANCED_CONTAINERS, try_decode(memory.data(), length));
}

TEST(DecodeInvalid, UnclosedMap)
{
    const int memory_size = 1000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process));
	int length = cbe_encode_get_buffer_offset(encode_process);
	EXPECT_EQ(CBE_ENCODE_STATUS_UNBALANCED_CONTAINERS, cbe_encode_end(encode_process));
	EXPECT_EQ(CBE_DECODE_STATUS_UNBALANCED_CONTAINERS, try_decode(memory.data(), length));
}

TEST(DecodeInvalid, EmptyKey)
{
    const int memory_size = 1000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_INCORRECT_KEY_TYPE, cbe_encode_add_empty(encode_process));
	int length = cbe_encode_get_buffer_offset(encode_process);
	cbe_encode_end(encode_process);
	memory.data()[length++] = TYPE_EMPTY;
	EXPECT_EQ(CBE_DECODE_STATUS_INCORRECT_KEY_TYPE, try_decode(memory.data(), length));
}

TEST(DecodeInvalid, ListKey)
{
    const int memory_size = 1000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_INCORRECT_KEY_TYPE, cbe_encode_begin_list(encode_process));
	int length = cbe_encode_get_buffer_offset(encode_process);
	cbe_encode_end(encode_process);
	memory.data()[length++] = TYPE_LIST;
	EXPECT_EQ(CBE_DECODE_STATUS_INCORRECT_KEY_TYPE, try_decode(memory.data(), length));
}

TEST(DecodeInvalid, MapKey)
{
    const int memory_size = 1000;
    std::array<uint8_t, memory_size> memory;

	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size());
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process));
	EXPECT_EQ(CBE_ENCODE_STATUS_INCORRECT_KEY_TYPE, cbe_encode_begin_map(encode_process));
	int length = cbe_encode_get_buffer_offset(encode_process);
	cbe_encode_end(encode_process);
	memory.data()[length++] = TYPE_MAP;
	EXPECT_EQ(CBE_DECODE_STATUS_INCORRECT_KEY_TYPE, try_decode(memory.data(), length));
}

#define DEFINE_MISSING_MAP_VALUE_TEST(NAME, TYPE, VALUE) \
TEST(DecodeInvalid, MissingMapValue_ ## NAME) \
{ \
    const int memory_size = 1000; \
    std::array<uint8_t, memory_size> memory; \
	cbe_encode_process* encode_process = cbe_encode_begin(memory.data(), memory.size()); \
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_begin_map(encode_process)); \
	EXPECT_EQ(CBE_ENCODE_STATUS_OK, cbe_encode_add_ ## TYPE(encode_process, VALUE)); \
	int length = cbe_encode_get_buffer_offset(encode_process); \
	EXPECT_EQ(CBE_ENCODE_STATUS_MISSING_VALUE_FOR_KEY, cbe_encode_end_container(encode_process)); \
	cbe_encode_end(encode_process); \
	memory.data()[length++] = TYPE_END_CONTAINER; \
	EXPECT_EQ(CBE_DECODE_STATUS_MISSING_VALUE_FOR_KEY, try_decode(memory.data(), length)); \
}
DEFINE_MISSING_MAP_VALUE_TEST(int8,        int_8,       1)
DEFINE_MISSING_MAP_VALUE_TEST(int16,       int_16,      1)
DEFINE_MISSING_MAP_VALUE_TEST(int32,       int_32,      1)
DEFINE_MISSING_MAP_VALUE_TEST(int64,       int_64,      1)
DEFINE_MISSING_MAP_VALUE_TEST(int128,      int_128,     1)
DEFINE_MISSING_MAP_VALUE_TEST(float32,     float_32,    1)
DEFINE_MISSING_MAP_VALUE_TEST(float64,     float_64,    1)
DEFINE_MISSING_MAP_VALUE_TEST(float128,    float_128,   1)
DEFINE_MISSING_MAP_VALUE_TEST(decimal32,   decimal_32,  1)
DEFINE_MISSING_MAP_VALUE_TEST(decimal64,   decimal_64,  1)
DEFINE_MISSING_MAP_VALUE_TEST(decimal128,  decimal_128, 1)
DEFINE_MISSING_MAP_VALUE_TEST(time,        time,        1)
DEFINE_MISSING_MAP_VALUE_TEST(string,      string,      "1")
DEFINE_MISSING_MAP_VALUE_TEST(long_string, string,      "this is a longer string for testing")

TEST(DecodeInvalid, MissingMapValue_Depth)
{
    const int memory_size = 1000;
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
	int length = cbe_encode_get_buffer_offset(encode_process);
	cbe_encode_end(encode_process);
	memory.data()[length++] = TYPE_END_CONTAINER;
	EXPECT_EQ(CBE_DECODE_STATUS_MISSING_VALUE_FOR_KEY, try_decode(memory.data(), length));
}

TEST(DecodeInvalid, ContainersTooDeep)
{
	const int depth_too_far = 10001;
    const int memory_size = 100000;
    std::array<uint8_t, memory_size> memory;

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

