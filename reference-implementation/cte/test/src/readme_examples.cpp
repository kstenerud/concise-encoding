#include <gtest/gtest.h>
#include <cte/cte.h>
#include <stdio.h>

TEST(CTE_Examples, encode)
{
    int indent_spaces = 4;
    uint8_t buffer[1000];
    cte_encode_context context = cte_new_encode_context_with_config(buffer,
                                                                        buffer + sizeof(buffer),
                                                                        indent_spaces,
                                                                        DEFAULT_FLOAT_DIGITS_PRECISION);

    cte_start_map(&context);
    cte_add_string(&context, "null");
    cte_add_empty(&context);
    cte_add_string(&context, "one");
    cte_add_int_64(&context, 1);
    cte_add_string(&context, "list");
        cte_start_list(&context);
        cte_add_int_64(&context, 1);
        cte_add_int_64(&context, 2);
        cte_add_int_64(&context, 3);
            cte_start_map(&context);
            cte_add_string(&context, "a");
            cte_add_int_64(&context, 1);
            cte_add_string(&context, "b");
            cte_add_int_64(&context, 2);
            cte_add_string(&context, "c");
            cte_add_int_64(&context, 3);
            cte_end_container(&context);
        cte_end_container(&context);
    cte_add_string(&context, "true");
    cte_add_boolean(&context, true);
    cte_end_container(&context);
    cte_end_encoding(&context);

    printf("%s\n", buffer);
}

typedef struct
{
    int depth;
} cte_parse_context;

static void indent(cte_parse_context* context)
{
    for(int i = 0; i < context->depth; i++)
    {
        printf("  ");
    }
}

static void on_error(void* context, const char* message)
{
    printf("Error: %s\n", message);
}

static bool on_string(void* context, const char* str)
{
    indent((cte_parse_context*)context);
    printf("\"%s\"\n", str);
    return true;
}

static bool on_empty(void* context)
{
    indent((cte_parse_context*)context);
    printf("empty\n");
    return true;
}
    
static bool on_boolean(void* context, bool value)
{
    indent((cte_parse_context*)context);
    printf(value ? "true\n" : "false\n");
    return true;
}
    
static bool on_int_64(void* context, int64_t value)
{
    indent((cte_parse_context*)context);
    if(value <= INT64_MAX)
    {
        printf("%ld\n", (int64_t)value);
    }
    else
    {
        // TODO
    }
    return true;
}
    
static bool on_float_64(void* context, double value)
{
    indent((cte_parse_context*)context);
    printf("%lg\n", value);
    return true;
}

static bool on_list_start(void* context)
{
    indent((cte_parse_context*)context);
    printf("[\n");
    ((cte_parse_context*)context)->depth++;
    return true;
}
    
static bool on_list_end(void* context)
{
    ((cte_parse_context*)context)->depth--;
    indent((cte_parse_context*)context);
    printf("]\n");
    return true;
}
    
static bool on_map_start(void* context)
{
    indent((cte_parse_context*)context);
    printf("{\n");
    ((cte_parse_context*)context)->depth++;
    return true;
}
    
static bool on_map_end(void* context)
{
    ((cte_parse_context*)context)->depth--;
    indent((cte_parse_context*)context);
    printf("}\n");
    return true;
}

TEST(CTE_Examples, parse)
{
    cte_parse_callbacks callbacks;
    callbacks.on_empty = on_empty;
    callbacks.on_boolean = on_boolean;
    callbacks.on_int_64 = on_int_64;
    callbacks.on_float_64 = on_float_64;
    callbacks.on_string = on_string;
    callbacks.on_error = on_error;
    callbacks.on_list_start = on_list_start;
    callbacks.on_list_end = on_list_end;
    callbacks.on_map_start = on_map_start;
    callbacks.on_map_end = on_map_end;

    cte_parse_context context = {0};
    const char* cte_string = "{\"null\":null,\"one\":1,\"list\":[1,2,3,{\"a\":1,\"b\":2,\"c\":3}],\"true\":true}";
    cte_parse_string(cte_string, &callbacks, &context);
}
