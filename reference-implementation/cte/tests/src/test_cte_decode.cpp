#include <gtest/gtest.h>
#include <cte/cte.h>
#include "decode_test_helpers.h"
#include "managed_allocator.h"
#include <stdarg.h>

void expect_decoded(const char* cte, ...)
{
    decode_test_context test_context =
    {
        .type = {TYPE_UNSET},
        .data = {0},
        .index = 0
    };
    managed_free_all();
    decode_test_context* context = &test_context;
    cte_decode_callbacks callbacks = decode_new_callbacks();

    std::vector<uint8_t> decode_process_backing_store(cte_decode_process_size());
    cte_decode_process* decode_process = (cte_decode_process*)decode_process_backing_store.data();
    ASSERT_EQ(CTE_DECODE_STATUS_OK, cte_decode_begin(decode_process, &callbacks, context));
    ASSERT_EQ(CTE_DECODE_STATUS_OK, cte_decode_feed(decode_process, cte, strlen(cte)));
    cte_decode_end(decode_process);
    fflush(stdout);

    va_list args;
    va_start(args, cte);
    int expected_arg_count = 0;
    for(;;expected_arg_count++)
    {
        int index = expected_arg_count;
        decoded_type expected_type = (decoded_type)va_arg(args, int);
        if(expected_type == TYPE_UNSET)
        {
            break;
        }
        decoded_type actual_type = decode_get_type(context, index);
        ASSERT_EQ(expected_type, actual_type);
        switch(expected_type)
        {
            case TYPE_BOOLEAN:
            {
                bool expected_value = (bool)va_arg(args, int);
                bool actual_value = decode_get_bool(context, index);
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
                const char* actual_value = decode_get_string(context, index);
                ASSERT_STREQ(expected_value, actual_value);
                break;
            }
            case TYPE_INT:
            {
                int64_t expected_value = va_arg(args, int64_t);
                int64_t actual_value = decode_get_int(context, index);
                ASSERT_EQ(expected_value, actual_value);
                break;
            }
            case TYPE_FLOAT:
            {
                double expected_value = va_arg(args, double);
                double actual_value = decode_get_float(context, index);
                ASSERT_EQ(expected_value, actual_value);
                break;
            }
            default:
                break;
        }
    }
    va_end(args);
    int actual_arg_count = decode_get_item_count(context);
    ASSERT_EQ(expected_arg_count, actual_arg_count);
}

void expect_decode_failure(const char* cte)
{
    decode_test_context test_context =
    {
        .type = {TYPE_UNSET},
        .data = {0},
        .index = 0
    };
    managed_free_all();
    void* context = &test_context;
    cte_decode_callbacks callbacks = decode_new_callbacks();

    std::vector<uint8_t> decode_process_backing_store(cte_decode_process_size());
    cte_decode_process* decode_process = (cte_decode_process*)decode_process_backing_store.data();
    ASSERT_EQ(CTE_DECODE_STATUS_OK, cte_decode_begin(decode_process, &callbacks, context));
    ASSERT_NE(CTE_DECODE_STATUS_OK, cte_decode_feed(decode_process, cte, strlen(cte)));
    cte_decode_end(decode_process);
}

TEST(CTE_Decode, string)
{
    expect_decoded("\"this is a string\"", TYPE_STRING, "this is a string", 0);
}

TEST(CTE_Decode, comment)
{
    expect_decoded("#\"this is a string\"\n\"a\"", TYPE_STRING, "a", 0);
}

TEST(CTE_Decode, string_multiline)
{
    expect_decoded("\"multi\nline\"", TYPE_STRING, "multi\nline", 0);
}

TEST(CTE_Decode, string_quoted)
{
    expect_decoded("\"\\\"quoted\\\"\"", TYPE_STRING, "\"quoted\"", 0);
}

TEST(CTE_Decode, string_tab)
{
    expect_decoded("\"tab\\ttab\"", TYPE_STRING, "tab\ttab", 0);
}

TEST(CTE_Decode, string_backslash)
{
    expect_decoded("\"bslash\\\\bslash\"", TYPE_STRING, "bslash\\bslash", 0);
}

TEST(CTE_Decode, string_r)
{
    expect_decoded("\"something\\rsomething\"", TYPE_STRING, "something\rsomething", 0);
}

TEST(CTE_Decode, string_unicode)
{
    expect_decoded("\"something\\u0001something\"", TYPE_STRING, "something\u0001something", 0);
    expect_decoded("\"something\\u0041something\"", TYPE_STRING, "somethingAsomething", 0);
    expect_decoded("\"something\\u00dfsomething\"", TYPE_STRING, "somethingßsomething", 0);
    expect_decoded("\"something\\u0419something\"", TYPE_STRING, "somethingЙsomething", 0);
    expect_decoded("\"something\\u306asomething\"", TYPE_STRING, "somethingなsomething", 0);
}



TEST(CTE_Decode, integer)
{
    expect_decoded("10000", TYPE_INT, (int64_t)10000, 0);
    expect_decoded("-5123", TYPE_INT, (int64_t)-5123, 0);
}

TEST(CTE_Decode, int_2)
{
    expect_decoded("1000b", TYPE_INT, (int64_t)8, 0);
}

TEST(CTE_Decode, int_8)
{
    expect_decoded("1000o", TYPE_INT, (int64_t)512, 0);
}

TEST(CTE_Decode, int_16)
{
    expect_decoded("1000h", TYPE_INT, (int64_t)4096, 0);
}

TEST(CTE_Decode, float)
{
    expect_decoded("1.1", TYPE_FLOAT, 1.1, 0);
    expect_decoded("-41.1964", TYPE_FLOAT, -41.1964, 0);
    expect_decoded("-1.84e9", TYPE_FLOAT, -1.84e9, 0);
    expect_decoded("-1.84e+9", TYPE_FLOAT, -1.84e+9, 0);
    expect_decoded("-1.84e-9", TYPE_FLOAT, -1.84e-9, 0);
}

TEST(CTE_Decode, boolean)
{
    expect_decoded("t", TYPE_BOOLEAN, (int)true, 0);
    expect_decoded("true", TYPE_BOOLEAN, (int)true, 0);
    expect_decoded("f", TYPE_BOOLEAN, (int)false, 0);
    expect_decoded("false", TYPE_BOOLEAN, (int)false, 0);
}

TEST(CTE_Decode, empty)
{
    expect_decoded("empty", TYPE_EMPTY, 0);
}

TEST(CTE_Decode, mixed)
{
    expect_decoded("{\"a\": [1 2 3] \"b\": false \"c\": empty}",
        TYPE_MAP_START,
            TYPE_STRING, "a", TYPE_LIST_START, TYPE_INT, 1, TYPE_INT, 2, TYPE_INT, 3, TYPE_LIST_END,
            TYPE_STRING, "b", TYPE_BOOLEAN, (int)false,
            TYPE_STRING, "c", TYPE_EMPTY,
        TYPE_MAP_END,
        0);
}

TEST(CTE_Decode, list_commas)
{
    expect_decoded("[1 2 ]", TYPE_LIST_START, TYPE_INT, 1, TYPE_INT, 2, TYPE_LIST_END, 0);
    expect_decoded("[ 1 2  3 ]", TYPE_LIST_START, TYPE_INT, 1, TYPE_INT, 2, TYPE_INT, 3, TYPE_LIST_END, 0);
}

TEST(CTE_Decode, map_commas)
{
    expect_decoded("{1: 10  2: 20 }", TYPE_MAP_START, TYPE_INT, 1, TYPE_INT, 10, TYPE_INT, 2, TYPE_INT, 20, TYPE_MAP_END, 0);
    expect_decoded("{1: 10  2: 20 3: 30}", TYPE_MAP_START, TYPE_INT, 1, TYPE_INT, 10, TYPE_INT, 2, TYPE_INT, 20, TYPE_INT, 3, TYPE_INT, 30, TYPE_MAP_END, 0);
}

TEST(CTE_Decode, fail_decode)
{
    expect_decode_failure("w{\"a\": [1 2 3]}");
}

TEST(CTE_Decode, fail_unbalanced_list)
{
    expect_decode_failure("[1 2 3");
}

TEST(CTE_Decode, fail_unbalanced_map)
{
    expect_decode_failure("{1: 10 2: 20 3: 30");
}

TEST(CTE_Decode, fail_bad_map)
{
    expect_decode_failure("{1: 10 2: 20 3}");
    expect_decode_failure("{1: 10 2: 20 3: }");
    expect_decode_failure("{1: 10 2: 20 3  30}");
}

TEST(CTE_Decode, fail_bad_slash)
{
    expect_decode_failure("\"\\\"");
    expect_decode_failure("\"\\q\"");
    expect_decode_failure("\"\\u\"");
    expect_decode_failure("\"\\u0\"");
    expect_decode_failure("\"\\u00\"");
    expect_decode_failure("\"\\u000\"");
}
