#include <gtest/gtest.h>
#include <cte/cte.h>

#define DEFINE_ENCODE_TEST(NAME, EXPECTED, ...) \
TEST(CTE_Encode, NAME) \
{ \
    const char* expected = EXPECTED; \
    uint8_t buff[1000]; \
    cte_encode_process* process = cte_encode_begin(buff, sizeof(buff)); \
    __VA_ARGS__ \
    fflush(stdout); \
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end(process)); \
    ASSERT_STREQ(expected, (const char*)buff); \
}

#define DEFINE_ENCODE_FLOAT_TEST(NAME, DIGITS, EXPECTED, ...) \
TEST(CTE_Encode, NAME) \
{ \
    const char* expected = EXPECTED; \
    uint8_t buff[1000]; \
    cte_encode_process* process = cte_encode_begin_with_config(buff, \
                                                                sizeof(buff), \
                                                                DEFAULT_INDENT_SPACES, \
                                                                DIGITS); \
    __VA_ARGS__ \
    fflush(stdout); \
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end(process)); \
    ASSERT_STREQ(expected, (const char*)buff); \
}

#define DEFINE_ENCODE_INDENTATION_TEST(NAME, INDENTATION, EXPECTED, ...) \
TEST(CTE_Encode, NAME) \
{ \
    const char* expected = EXPECTED; \
    uint8_t buff[1000]; \
    cte_encode_process* process = cte_encode_begin_with_config(buff, \
                                                                sizeof(buff), \
                                                                INDENTATION, \
                                                                DEFAULT_FLOAT_DIGITS_PRECISION); \
    __VA_ARGS__ \
    fflush(stdout); \
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end(process)); \
    ASSERT_STREQ(expected, (const char*)buff); \
}

#define DEFINE_ENCODE_FAIL_TEST(NAME, BUFFER_SIZE, ...) \
TEST(CTE_Encode, NAME) \
{ \
    uint8_t buff[BUFFER_SIZE]; \
    cte_encode_process* process = cte_encode_begin(buff, sizeof(buff)); \
    __VA_ARGS__ \
    fflush(stdout); \
}

DEFINE_ENCODE_TEST(empty, "empty", { ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_empty(process)); })
DEFINE_ENCODE_TEST(false, "f", { ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_boolean(process, false)); })
DEFINE_ENCODE_TEST(true, "t", { ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_boolean(process, true)); })
DEFINE_ENCODE_TEST(int_1000, "1000", { ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 1000)); })
DEFINE_ENCODE_TEST(int_9223372036854775807, "9223372036854775807", { ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 9223372036854775807L)); })
DEFINE_ENCODE_TEST(int_n9223372036854775807, "-9223372036854775807", { ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, -9223372036854775807L)); })
DEFINE_ENCODE_TEST(float_1_1, "1.1", { ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_float_64(process, 1.1)); })
DEFINE_ENCODE_TEST(float_924_5122045, "924.5122045", { ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_float_64(process, 924.5122045)); })
DEFINE_ENCODE_TEST(string, "\"a string\"", { ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_string(process, "a string")); })
DEFINE_ENCODE_TEST(escaped, "\"q\\\"s\\\\b\\bf\\fn\\nr\\rt\\t\"", { ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_string(process, "q\"s\\b\bf\fn\nr\rt\t")); })

DEFINE_ENCODE_FLOAT_TEST(float_limit_10, 10, "1.012345679", { ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_float_64(process, 1.0123456789)); })

DEFINE_ENCODE_TEST(substring, "\"a string\"",
{
    const char* string = "a string that's a substring";
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_substring(process, string, 8));
})

DEFINE_ENCODE_TEST(list, "[1,2,3]",
{
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_list(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 1));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 2));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 3));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end_container(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end(process));
})

DEFINE_ENCODE_TEST(empty_list, "[]",
{
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_list(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end_container(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end(process));
})

DEFINE_ENCODE_TEST(single_list, "[1]",
{
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_list(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 1));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end_container(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end(process));
})

DEFINE_ENCODE_TEST(map, "{\"a\":1,\"b\":2,\"c\":3}",
{
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_map(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_string(process, "a"));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 1));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_string(process, "b"));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 2));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_string(process, "c"));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 3));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end_container(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end(process));
})

DEFINE_ENCODE_TEST(empty_map, "{}",
{
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_map(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end_container(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end(process));
})

DEFINE_ENCODE_TEST(single_map, "{\"a\":1}",
{
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_map(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_string(process, "a"));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 1));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end_container(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end(process));
})

DEFINE_ENCODE_TEST(complex, "{\"empty\":empty,\"one\":1,\"list\":[1,2,3,{\"a\":1,\"b\":2,\"c\":3}],\"true\":t}",
{
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_map(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_string(process, "empty"));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_empty(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_string(process, "one"));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 1));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_string(process, "list"));
        ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_list(process));
        ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 1));
        ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 2));
        ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 3));
            ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_map(process));
            ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_string(process, "a"));
            ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 1));
            ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_string(process, "b"));
            ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 2));
            ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_string(process, "c"));
            ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 3));
            ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end_container(process));
        ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end_container(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_string(process, "true"));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_boolean(process, true));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end_container(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end(process));
})

DEFINE_ENCODE_FAIL_TEST(fail_int_size_0,0,
{
    ASSERT_NE(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 1));
})

DEFINE_ENCODE_FAIL_TEST(fail_int_size_1,1,
{
    ASSERT_NE(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 10));
})

DEFINE_ENCODE_FAIL_TEST(fail_float_size_1,1,
{
    ASSERT_NE(CTE_ENCODE_STATUS_OK, cte_encode_add_float_64(process, 0.1));
})

DEFINE_ENCODE_FAIL_TEST(fail_string_size_10,10,
{
    ASSERT_NE(CTE_ENCODE_STATUS_OK, cte_encode_add_string(process, "this is a test"));
})

DEFINE_ENCODE_FAIL_TEST(fail_empty,3,
{
    ASSERT_NE(CTE_ENCODE_STATUS_OK, cte_encode_add_empty(process));
})

DEFINE_ENCODE_FAIL_TEST(fail_completion,1,
{
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_boolean(process, false));
    ASSERT_NE(CTE_ENCODE_STATUS_OK, cte_encode_end(process)); \
})

DEFINE_ENCODE_FAIL_TEST(fail_list,0,
{
    ASSERT_NE(CTE_ENCODE_STATUS_OK, cte_encode_begin_list(process));
})

DEFINE_ENCODE_FAIL_TEST(fail_map,0,
{
    ASSERT_NE(CTE_ENCODE_STATUS_OK, cte_encode_begin_map(process));
})

DEFINE_ENCODE_FAIL_TEST(fail_close_container,1,
{
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_list(process));
    ASSERT_NE(CTE_ENCODE_STATUS_OK, cte_encode_end_container(process));
})

DEFINE_ENCODE_FAIL_TEST(fail_end_encoding,4,
{
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_list(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_map(process));
    ASSERT_NE(CTE_ENCODE_STATUS_OK, cte_encode_end(process));
})

DEFINE_ENCODE_FAIL_TEST(fail_early_map_close,100,
{
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_map(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_string(process, "a"));
    ASSERT_NE(CTE_ENCODE_STATUS_OK, cte_encode_end_container(process));
})

DEFINE_ENCODE_FAIL_TEST(fail_too_many_list_closes,100,
{
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_list(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end_container(process));
    ASSERT_NE(CTE_ENCODE_STATUS_OK, cte_encode_end_container(process));
})

DEFINE_ENCODE_FAIL_TEST(fail_too_many_map_closes,100,
{
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_map(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end_container(process));
    ASSERT_NE(CTE_ENCODE_STATUS_OK, cte_encode_end_container(process));
})

DEFINE_ENCODE_FAIL_TEST(fail_map_key_is_empty,100,
{
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_map(process));
    ASSERT_NE(CTE_ENCODE_STATUS_OK, cte_encode_add_empty(process));
})

DEFINE_ENCODE_FAIL_TEST(fail_map_key_is_boolean,100,
{
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_map(process));
    ASSERT_NE(CTE_ENCODE_STATUS_OK, cte_encode_add_boolean(process, true));
})

DEFINE_ENCODE_FAIL_TEST(fail_map_key_is_integer,100,
{
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_map(process));
    ASSERT_NE(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 1));
})

DEFINE_ENCODE_FAIL_TEST(fail_map_key_is_float,100,
{
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_map(process));
    ASSERT_NE(CTE_ENCODE_STATUS_OK, cte_encode_add_float_64(process, 0.1));
})

DEFINE_ENCODE_FAIL_TEST(fail_map_key_is_map,100,
{
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_map(process));
    ASSERT_NE(CTE_ENCODE_STATUS_OK, cte_encode_begin_map(process));
})

DEFINE_ENCODE_FAIL_TEST(fail_map_key_is_list,100,
{
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_map(process));
    ASSERT_NE(CTE_ENCODE_STATUS_OK, cte_encode_begin_list(process));
})

DEFINE_ENCODE_INDENTATION_TEST(indent_list, 2, "[\n  1,\n  2,\n  3\n]",
{
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_list(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 1));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 2));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 3));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end_container(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end(process));
})

DEFINE_ENCODE_INDENTATION_TEST(indent_map_list, 4, "{\n    \"number\": 1,\n    \"list\": [\n        10\n    ]\n}",
{
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_map(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_string(process, "number"));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 1));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_string(process, "list"));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_begin_list(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_add_int_64(process, 10));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end_container(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end_container(process));
    ASSERT_EQ(CTE_ENCODE_STATUS_OK, cte_encode_end(process));
})
