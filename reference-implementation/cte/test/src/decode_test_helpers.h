#ifndef decode_test_helpers_H
#define decode_test_helpers_H
#ifdef __cplusplus
extern "C" {
#endif


#include <cte/cte.h>

typedef enum
{
    TYPE_UNSET = 0,
    TYPE_ERROR,
    TYPE_UNEXPECTED,
    TYPE_EMPTY,
    TYPE_BOOLEAN,
    TYPE_STRING,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_LIST_START,
    TYPE_LIST_END,
    TYPE_MAP_START,
    TYPE_MAP_END,
} decoded_type;

typedef struct
{
    decoded_type type[100000];
    void* data[100000];
    int index;
} decode_test_context;


int decode_get_item_count(decode_test_context* context);
decoded_type decode_get_type(decode_test_context* context, int index);
const char* decode_get_string(decode_test_context* context, int index);
int64_t decode_get_int(decode_test_context* context, int index);
double decode_get_float(decode_test_context* context, int index);
bool decode_get_bool(decode_test_context* context, int index);
void decode_add_type(decode_test_context* context, decoded_type type);
void decode_add_string(decode_test_context* context, decoded_type type, const char* value);
void decode_add_int(decode_test_context* context, int64_t value);
void decode_add_float(decode_test_context* context, double value);
void decode_add_bool(decode_test_context* context, bool value);

cte_decode_callbacks decode_new_callbacks();


#ifdef __cplusplus 
}
#endif
#endif // decode_test_helpers_H
