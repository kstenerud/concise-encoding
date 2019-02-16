#include "decode_test_helpers.h"
#include "managed_allocator.h"
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

static decode_test_context* decode_get_context(struct cte_decode_process* process)
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

static void on_error(struct cte_decode_process* process, const char* message)
{
    printf("Error = %s\n", message);
    decode_add_string(decode_get_context(process), TYPE_ERROR, message);
}

static bool on_string_begin(struct cte_decode_process* process)
{
    (void)process;
    // Nothing to do
    return true;
}

static bool on_string_data(struct cte_decode_process* process, const char* str, int64_t byte_count)
{
    // TODO
    (void)byte_count;
    return decode_add_string(decode_get_context(process), TYPE_STRING, str);
}

static bool on_string_end(struct cte_decode_process* process)
{
    (void)process;
    // TODO
    return true;
}

static bool on_binary_begin(struct cte_decode_process* process)
{
    (void)process;
    // Nothing to do
    return true;
}

static bool on_binary_data(struct cte_decode_process* process, const uint8_t* data, int64_t byte_count)
{
    // TODO
    (void)process;
    (void)data;
    (void)byte_count;
    return true;
}

static bool on_binary_end(struct cte_decode_process* process)
{
    (void)process;
    // TODO
    return true;
}

static bool on_nil(struct cte_decode_process* process)
{
    return decode_add_type(decode_get_context(process), TYPE_NIL);
}
    
static bool on_boolean(struct cte_decode_process* process, bool value)
{
    return decode_add_bool(decode_get_context(process), value);
}
    
static bool on_int_8(struct cte_decode_process* process, int8_t value)
{
    return decode_add_int(decode_get_context(process), value);
}
    
static bool on_int_16(struct cte_decode_process* process, int16_t value)
{
    return decode_add_int(decode_get_context(process), value);
}
    
static bool on_int_32(struct cte_decode_process* process, int32_t value)
{
    return decode_add_int(decode_get_context(process), value);
}
    
static bool on_int_64(struct cte_decode_process* process, int64_t value)
{
    return decode_add_int(decode_get_context(process), value);
}
    
static bool on_int_128(struct cte_decode_process* process, __int128 value)
{
    return decode_add_int(decode_get_context(process), value);
}

static bool on_float_32(struct cte_decode_process* process, float value)
{
    return decode_add_float(decode_get_context(process), value);
}

static bool on_float_64(struct cte_decode_process* process, double value)
{
    return decode_add_float(decode_get_context(process), value);
}

static bool on_float_128(struct cte_decode_process* process, __float128 value)
{
    return decode_add_float(decode_get_context(process), value);
}

static bool on_decimal_32(struct cte_decode_process* process, _Decimal32 value)
{
    // TODO
    (void)process;
    (void)value;
    return true;
}

static bool on_decimal_64(struct cte_decode_process* process, _Decimal64 value)
{
    // TODO
    (void)process;
    (void)value;
    return true;
}

static bool on_decimal_128(struct cte_decode_process* process, _Decimal128 value)
{
    // TODO
    (void)process;
    (void)value;
    return true;
}

static bool on_time(struct cte_decode_process* process, const char* value)
{
    // TODO
    (void)process;
    (void)value;
    return true;
}

static bool on_list_begin(struct cte_decode_process* process)
{
    return decode_add_type(decode_get_context(process), TYPE_LIST_START);
}
    
static bool on_list_end(struct cte_decode_process* process)
{
    return decode_add_type(decode_get_context(process), TYPE_LIST_END);
}
    
static bool on_map_begin(struct cte_decode_process* process)
{
    return decode_add_type(decode_get_context(process), TYPE_MAP_START);
}
    
static bool on_map_end(struct cte_decode_process* process)
{
    return decode_add_type(decode_get_context(process), TYPE_MAP_END);
}

cte_decode_callbacks decode_new_callbacks()
{
    cte_decode_callbacks callbacks = {
        .on_error = on_error,
        .on_nil = on_nil,
        .on_boolean = on_boolean,
        .on_int_8 = on_int_8,
        .on_int_16 = on_int_16,
        .on_int_32 = on_int_32,
        .on_int_64 = on_int_64,
        .on_int_128 = on_int_128,
        .on_float_32 = on_float_32,
        .on_float_64 = on_float_64,
        .on_float_128 = on_float_128,
        .on_decimal_32 = on_decimal_32,
        .on_decimal_64 = on_decimal_64,
        .on_decimal_128 = on_decimal_128,
        .on_time = on_time,
        .on_list_begin = on_list_begin,
        .on_list_end = on_list_end,
        .on_map_begin = on_map_begin,
        .on_map_end = on_map_end,
        .on_string_begin = on_string_begin,
        .on_string_data = on_string_data,
        .on_string_end = on_string_end,
        .on_binary_begin = on_binary_begin,
        .on_binary_data = on_binary_data,
        .on_binary_end = on_binary_end,
    };
	return callbacks;	
}
