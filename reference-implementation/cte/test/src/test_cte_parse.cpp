#include <gtest/gtest.h>
#include <cte/cte.h>
#include "parse_test_helpers.h"
#include "managed_allocator.h"
#include <stdarg.h>

void expect_decoded(const char* cte, ...)
{
    parse_test_context test_context =
    {
        .type = {TYPE_UNSET},
        .data = {0},
        .index = 0
    };
    managed_free_all();
    parse_test_context* context = &test_context;
    cte_parse_callbacks callbacks = parse_new_callbacks();

    bool result = cte_parse_string(cte, &callbacks, context);
    fflush(stdout);
    ASSERT_TRUE(result);

    va_list args;
    va_start(args, cte);
    int expected_arg_count = 0;
    for(;;expected_arg_count++)
    {
        int index = expected_arg_count;
        parsed_type expected_type = (parsed_type)va_arg(args, int);
        if(expected_type == TYPE_UNSET)
        {
            break;
        }
        parsed_type actual_type = parse_get_type(context, index);
        ASSERT_EQ(expected_type, actual_type);
        switch(expected_type)
        {
            case TYPE_BOOLEAN:
            {
                bool expected_value = (bool)va_arg(args, int);
                bool actual_value = parse_get_bool(context, index);
                if(expected_value)
                {
                    ASSERT_TRUE(actual_value);
                }
                else
                {
                    ASSERT_FALSE(actual_value);
                }
                break;
            }
            case TYPE_STRING:
            {
                const char* expected_value = va_arg(args, char*);
                const char* actual_value = parse_get_string(context, index);
                ASSERT_STREQ(expected_value, actual_value);
                break;
            }
            case TYPE_INT:
            {
                int64_t expected_value = va_arg(args, int64_t);
                int64_t actual_value = parse_get_int(context, index);
                ASSERT_EQ(expected_value, actual_value);
                break;
            }
            case TYPE_FLOAT:
            {
                double expected_value = va_arg(args, double);
                double actual_value = parse_get_float(context, index);
                ASSERT_EQ(expected_value, actual_value);
                break;
            }
            default:
                break;
        }
    }
    va_end(args);
    int actual_arg_count = parse_get_item_count(context);
    ASSERT_EQ(expected_arg_count, actual_arg_count);
}

void expect_decode_failure(const char* cte)
{
    parse_test_context test_context =
    {
        .type = {TYPE_UNSET},
        .data = {0},
        .index = 0
    };
    managed_free_all();
    void* context = &test_context;
    cte_parse_callbacks callbacks = parse_new_callbacks();

    bool result = cte_parse_string(cte, &callbacks, context);
    fflush(stdout);
    ASSERT_FALSE(result);
}

TEST(CTE_Parse, string)
{
    expect_decoded("\"this is a string\"", TYPE_STRING, "this is a string", 0);
}

TEST(CTE_Parse, comment)
{
    expect_decoded("#\"this is a string\"\n\"a\"", TYPE_STRING, "a", 0);
}

TEST(CTE_Parse, string_multiline)
{
    expect_decoded("\"multi\nline\"", TYPE_STRING, "multi\nline", 0);
}

TEST(CTE_Parse, string_quoted)
{
    expect_decoded("\"\\\"quoted\\\"\"", TYPE_STRING, "\"quoted\"", 0);
}

TEST(CTE_Parse, string_tab)
{
    expect_decoded("\"tab\\ttab\"", TYPE_STRING, "tab\ttab", 0);
}

TEST(CTE_Parse, string_backslash)
{
    expect_decoded("\"bslash\\\\bslash\"", TYPE_STRING, "bslash\\bslash", 0);
}

TEST(CTE_Parse, string_r)
{
    expect_decoded("\"something\\rsomething\"", TYPE_STRING, "something\rsomething", 0);
}

TEST(CTE_Parse, string_unicode)
{
    expect_decoded("\"something\\u0001something\"", TYPE_STRING, "something\u0001something", 0);
    expect_decoded("\"something\\u0041something\"", TYPE_STRING, "somethingAsomething", 0);
    expect_decoded("\"something\\u00dfsomething\"", TYPE_STRING, "somethingßsomething", 0);
    expect_decoded("\"something\\u0419something\"", TYPE_STRING, "somethingЙsomething", 0);
    expect_decoded("\"something\\u306asomething\"", TYPE_STRING, "somethingなsomething", 0);
}



TEST(CTE_Parse, integer)
{
    expect_decoded("10000", TYPE_INT, (int64_t)10000, 0);
    expect_decoded("-5123", TYPE_INT, (int64_t)-5123, 0);
}

TEST(CTE_Parse, int_2)
{
    expect_decoded("1000b", TYPE_INT, (int64_t)8, 0);
}

TEST(CTE_Parse, int_8)
{
    expect_decoded("1000o", TYPE_INT, (int64_t)512, 0);
}

TEST(CTE_Parse, int_16)
{
    expect_decoded("1000h", TYPE_INT, (int64_t)4096, 0);
}

TEST(CTE_Parse, float)
{
    expect_decoded("1.1", TYPE_FLOAT, 1.1, 0);
    expect_decoded("-41.1964", TYPE_FLOAT, -41.1964, 0);
    expect_decoded("-1.84e9", TYPE_FLOAT, -1.84e9, 0);
    expect_decoded("-1.84e+9", TYPE_FLOAT, -1.84e+9, 0);
    expect_decoded("-1.84e-9", TYPE_FLOAT, -1.84e-9, 0);
}

TEST(CTE_Parse, boolean)
{
    expect_decoded("t", TYPE_BOOLEAN, (int)true, 0);
    expect_decoded("true", TYPE_BOOLEAN, (int)true, 0);
    expect_decoded("f", TYPE_BOOLEAN, (int)false, 0);
    expect_decoded("false", TYPE_BOOLEAN, (int)false, 0);
}

TEST(CTE_Parse, empty)
{
    expect_decoded("empty", TYPE_EMPTY, 0);
}

TEST(CTE_Parse, mixed)
{
    expect_decoded("{\"a\": [1 2 3] \"b\": false \"c\": empty}",
        TYPE_MAP_START,
            TYPE_STRING, "a", TYPE_LIST_START, TYPE_INT, 1, TYPE_INT, 2, TYPE_INT, 3, TYPE_LIST_END,
            TYPE_STRING, "b", TYPE_BOOLEAN, (int)false,
            TYPE_STRING, "c", TYPE_EMPTY,
        TYPE_MAP_END,
        0);
}

TEST(CTE_Parse, list_commas)
{
    expect_decoded("[1 2 ]", TYPE_LIST_START, TYPE_INT, 1, TYPE_INT, 2, TYPE_LIST_END, 0);
    expect_decoded("[ 1 2  3 ]", TYPE_LIST_START, TYPE_INT, 1, TYPE_INT, 2, TYPE_INT, 3, TYPE_LIST_END, 0);
}

TEST(CTE_Parse, map_commas)
{
    expect_decoded("{1: 10  2: 20 }", TYPE_MAP_START, TYPE_INT, 1, TYPE_INT, 10, TYPE_INT, 2, TYPE_INT, 20, TYPE_MAP_END, 0);
    expect_decoded("{1: 10  2: 20 3: 30}", TYPE_MAP_START, TYPE_INT, 1, TYPE_INT, 10, TYPE_INT, 2, TYPE_INT, 20, TYPE_INT, 3, TYPE_INT, 30, TYPE_MAP_END, 0);
}

TEST(CTE_Parse, fail_parse)
{
    expect_decode_failure("w{\"a\": [1 2 3]}");
}

TEST(CTE_Parse, fail_unbalanced_list)
{
    expect_decode_failure("[1 2 3");
}

TEST(CTE_Parse, fail_unbalanced_map)
{
    expect_decode_failure("{1: 10 2: 20 3: 30");
}

TEST(CTE_Parse, fail_bad_map)
{
    expect_decode_failure("{1: 10 2: 20 3}");
    expect_decode_failure("{1: 10 2: 20 3: }");
    expect_decode_failure("{1: 10 2: 20 3  30}");
}

TEST(CTE_Parse, fail_bad_slash)
{
    expect_decode_failure("\"\\\"");
    expect_decode_failure("\"\\q\"");
    expect_decode_failure("\"\\u\"");
    expect_decode_failure("\"\\u0\"");
    expect_decode_failure("\"\\u00\"");
    expect_decode_failure("\"\\u000\"");
}
