#ifndef decode_test_helpers_H
#define decode_test_helpers_H
#ifdef __cplusplus
extern "C" {
#endif


#include <cte/cte.h>

typedef enum
{
    TYPE_UNSET      = 0,
    TYPE_ERROR      = 1,
    TYPE_UNEXPECTED = 2,
    TYPE_NIL        = 3,
    TYPE_BOOLEAN    = 4,
    TYPE_STRING     = 5,
    TYPE_INT        = 6,
    TYPE_FLOAT      = 7,
    TYPE_LIST_START = 8,
    TYPE_LIST_END   = 9,
    TYPE_MAP_START  = 10,
    TYPE_MAP_END    = 11,
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

cte_decode_callbacks decode_new_callbacks();


#ifdef __cplusplus 
}
#endif
#endif // decode_test_helpers_H
