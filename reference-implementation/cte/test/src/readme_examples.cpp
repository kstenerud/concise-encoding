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
    cte_add_null(&context);
    cte_add_string(&context, "one");
    cte_add_integer(&context, 1);
    cte_add_string(&context, "list");
        cte_start_list(&context);
        cte_add_integer(&context, 1);
        cte_add_integer(&context, 2);
        cte_add_integer(&context, 3);
            cte_start_map(&context);
            cte_add_string(&context, "a");
            cte_add_integer(&context, 1);
            cte_add_string(&context, "b");
            cte_add_integer(&context, 2);
            cte_add_string(&context, "c");
            cte_add_integer(&context, 3);
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
} json_parse_context;

static void indent(json_parse_context* context)
{
    for(int i = 0; i < context->depth; i++)
    {
        printf("  ");
    }
}

static void on_parse_error(void* context, const char* message)
{
    printf("Error: %s\n", message);
}

static void on_string(void* context, const char* str)
{
    indent((json_parse_context*)context);
    printf("\"%s\"\n", str);
}

static void on_null(void* context)
{
    indent((json_parse_context*)context);
    printf("null\n");
}
    
static void on_bool(void* context, bool value)
{
    indent((json_parse_context*)context);
    printf(value ? "true\n" : "false\n");
}
    
static void on_int(void* context, int64_t value)
{
    indent((json_parse_context*)context);
    printf("%ld\n", value);
}
    
static void on_float(void* context, double value)
{
    indent((json_parse_context*)context);
    printf("%lg\n", value);
}

static void on_list_start(void* context)
{
    indent((json_parse_context*)context);
    printf("[\n");
    ((json_parse_context*)context)->depth++;
}
    
static void on_list_end(void* context)
{
    ((json_parse_context*)context)->depth--;
    indent((json_parse_context*)context);
    printf("]\n");
}
    
static void on_map_start(void* context)
{
    indent((json_parse_context*)context);
    printf("{\n");
    ((json_parse_context*)context)->depth++;
}
    
static void on_map_end(void* context)
{
    ((json_parse_context*)context)->depth--;
    indent((json_parse_context*)context);
    printf("}\n");
}

TEST(CTE_Examples, parse)
{
    cte_parse_callbacks callbacks;
    callbacks.on_null = on_null;
    callbacks.on_boolean = on_bool;
    callbacks.on_int = on_int;
    callbacks.on_float = on_float;
    callbacks.on_string = on_string;
    callbacks.on_parse_error = on_parse_error;
    callbacks.on_list_start = on_list_start;
    callbacks.on_list_end = on_list_end;
    callbacks.on_map_start = on_map_start;
    callbacks.on_map_end = on_map_end;

    json_parse_context context = {0};
    const char* json_string = "{\"null\":null,\"one\":1,\"list\":[1,2,3,{\"a\":1,\"b\":2,\"c\":3}],\"true\":true}";
    cte_parse_string(json_string, &callbacks, &context);
}
