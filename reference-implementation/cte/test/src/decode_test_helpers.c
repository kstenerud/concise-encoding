#include "decode_test_helpers.h"
#include "managed_allocator.h"
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

static decode_test_context* decode_get_context(cte_decode_process* process)
{
    return (decode_test_context*)cte_decode_get_user_context(process);
}

int decode_get_item_count(decode_test_context* context)
{
    return context->index;
}

decoded_type decode_get_type(decode_test_context* context, int index)
{
    return context->type[index];
}

const char* decode_get_string(decode_test_context* context, int index)
{
    return (const char*)context->data[index];
}

int64_t decode_get_int(decode_test_context* context, int index)
{
    return *((int64_t*)context->data[index]);
}

double decode_get_float(decode_test_context* context, int index)
{
    return *((double*)context->data[index]);
}

bool decode_get_bool(decode_test_context* context, int index)
{
    return *((bool*)context->data[index]);
}

static bool decode_add_type(decode_test_context* context, decoded_type type)
{
    context->type[context->index] = type;
    context->index++;
    return true;
}

static bool decode_add_string(decode_test_context* context, decoded_type type, const char* value)
{
    char* newvalue = (char*)managed_allocate(strlen(value) + 1);
    strcpy(newvalue, value);
    context->data[context->index] = newvalue;
    decode_add_type(context, type);
    return true;
}

static bool decode_add_int(decode_test_context* context, int64_t value)
{
    void* newvalue = managed_allocate(sizeof(value));
    memcpy(newvalue, &value, sizeof(value));
    context->data[context->index] = newvalue;
    decode_add_type(context, TYPE_INT);
    return true;
}

static bool decode_add_float(decode_test_context* context, double value)
{
    void* newvalue = managed_allocate(sizeof(value));
    memcpy(newvalue, &value, sizeof(value));
    context->data[context->index] = newvalue;
    decode_add_type(context, TYPE_FLOAT);
    return true;
}

static bool decode_add_bool(decode_test_context* context, bool value)
{
    void* newvalue = managed_allocate(sizeof(value));
    memcpy(newvalue, &value, sizeof(value));
    context->data[context->index] = newvalue;
    decode_add_type(context, TYPE_BOOLEAN);
    return true;
}

static void on_error(cte_decode_process* decode_process, const char* message)
{
    printf("Error: %s\n", message);
    decode_add_string(decode_get_context(decode_process), TYPE_ERROR, message);
}

static bool on_string(cte_decode_process* decode_process, const char* str)
{
    return decode_add_string(decode_get_context(decode_process), TYPE_STRING, str);
}

static bool on_empty(cte_decode_process* decode_process)
{
    return decode_add_type(decode_get_context(decode_process), TYPE_EMPTY);
}
    
static bool on_bool(cte_decode_process* decode_process, bool value)
{
    return decode_add_bool(decode_get_context(decode_process), value);
}
    
static bool on_int_64(cte_decode_process* decode_process, int64_t value)
{
    return decode_add_int(decode_get_context(decode_process), value);
}
    
static bool on_float_64(cte_decode_process* decode_process, double value)
{
    return decode_add_float(decode_get_context(decode_process), value);
}

static bool on_list_begin(cte_decode_process* decode_process)
{
    return decode_add_type(decode_get_context(decode_process), TYPE_LIST_START);
}
    
static bool on_list_end(cte_decode_process* decode_process)
{
    return decode_add_type(decode_get_context(decode_process), TYPE_LIST_END);
}
    
static bool on_map_begin(cte_decode_process* decode_process)
{
    return decode_add_type(decode_get_context(decode_process), TYPE_MAP_START);
}
    
static bool on_map_end(cte_decode_process* decode_process)
{
    return decode_add_type(decode_get_context(decode_process), TYPE_MAP_END);
}

cte_decode_callbacks decode_new_callbacks()
{
    cte_decode_callbacks callbacks;
    // sorry, unimplemented: non-trivial designated initializers not supported
        callbacks.on_empty = on_empty;
        callbacks.on_boolean = on_bool;
        callbacks.on_int_64 = on_int_64;
        callbacks.on_float_64 = on_float_64;
        callbacks.on_string = on_string;
        callbacks.on_error = on_error;
        callbacks.on_list_begin = on_list_begin;
        callbacks.on_list_end = on_list_end;
        callbacks.on_map_begin = on_map_begin;
        callbacks.on_map_end = on_map_end;
	return callbacks;	
}
