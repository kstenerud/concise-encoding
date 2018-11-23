#include "parse_test_helpers.h"
#include "managed_allocator.h"
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>


int parse_get_item_count(parse_test_context* context)
{
    return context->index;
}

parsed_type parse_get_type(parse_test_context* context, int index)
{
    return context->type[index];
}

const char* parse_get_string(parse_test_context* context, int index)
{
    return (const char*)context->data[index];
}

int64_t parse_get_int(parse_test_context* context, int index)
{
    return *((int64_t*)context->data[index]);
}

double parse_get_float(parse_test_context* context, int index)
{
    return *((double*)context->data[index]);
}

bool parse_get_bool(parse_test_context* context, int index)
{
    return *((bool*)context->data[index]);
}

void parse_add_type(parse_test_context* context, parsed_type type)
{
    context->type[context->index] = type;
    context->index++;
}

void parse_add_string(parse_test_context* context, parsed_type type, const char* value)
{
    char* newvalue = (char*)managed_allocate(strlen(value) + 1);
    strcpy(newvalue, value);
    context->data[context->index] = newvalue;
    parse_add_type(context, type);
}

void parse_add_int(parse_test_context* context, int64_t value)
{
    void* newvalue = managed_allocate(sizeof(value));
    memcpy(newvalue, &value, sizeof(value));
    context->data[context->index] = newvalue;
    parse_add_type(context, TYPE_INT);
}

void parse_add_float(parse_test_context* context, double value)
{
    void* newvalue = managed_allocate(sizeof(value));
    memcpy(newvalue, &value, sizeof(value));
    context->data[context->index] = newvalue;
    parse_add_type(context, TYPE_FLOAT);
}

void parse_add_bool(parse_test_context* context, bool value)
{
    void* newvalue = managed_allocate(sizeof(value));
    memcpy(newvalue, &value, sizeof(value));
    context->data[context->index] = newvalue;
    parse_add_type(context, TYPE_BOOLEAN);
}

static void on_error(void* context, const char* message)
{
    printf("Error: %s\n", message);
    parse_add_string((parse_test_context*)context, TYPE_ERROR, message);
}

static bool on_string(void* context, const char* str)
{
    parse_add_string((parse_test_context*)context, TYPE_STRING, str);
}

static bool on_empty(void* context)
{
    parse_add_type((parse_test_context*)context, TYPE_EMPTY);
}
    
static bool on_bool(void* context, bool value)
{
    parse_add_bool((parse_test_context*)context, value);
}
    
static bool on_int_64(void* context, int64_t value)
{
    parse_add_int((parse_test_context*)context, value);
}
    
static bool on_float_64(void* context, double value)
{
    parse_add_float((parse_test_context*)context, value);
}

static bool on_list_start(void* context)
{
    parse_add_type((parse_test_context*)context, TYPE_LIST_START);
}
    
static bool on_list_end(void* context)
{
    parse_add_type((parse_test_context*)context, TYPE_LIST_END);
}
    
static bool on_map_start(void* context)
{
    parse_add_type((parse_test_context*)context, TYPE_MAP_START);
}
    
static bool on_map_end(void* context)
{
    parse_add_type((parse_test_context*)context, TYPE_MAP_END);
}

cte_parse_callbacks parse_new_callbacks()
{
    cte_parse_callbacks callbacks;
    // sorry, unimplemented: non-trivial designated initializers not supported
        callbacks.on_empty = on_empty;
        callbacks.on_boolean = on_bool;
        callbacks.on_int_64 = on_int_64;
        callbacks.on_float_64 = on_float_64;
        callbacks.on_string = on_string;
        callbacks.on_error = on_error;
        callbacks.on_list_start = on_list_start;
        callbacks.on_list_end = on_list_end;
        callbacks.on_map_start = on_map_start;
        callbacks.on_map_end = on_map_end;
	return callbacks;	
}
