#ifndef parse_test_helpers_H
#define parse_test_helpers_H
#ifdef __cplusplus
extern "C" {
#endif


#include <cte/cte.h>

typedef enum
{
    TYPE_UNSET = 0,
    TYPE_ERROR,
    TYPE_UNEXPECTED,
    TYPE_NULL,
    TYPE_BOOLEAN,
    TYPE_STRING,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_LIST_START,
    TYPE_LIST_END,
    TYPE_MAP_START,
    TYPE_MAP_END,
} parsed_type;

typedef struct
{
    parsed_type type[100000];
    void* data[100000];
    int index;
} parse_test_context;


int parse_get_item_count(parse_test_context* context);
parsed_type parse_get_type(parse_test_context* context, int index);
const char* parse_get_string(parse_test_context* context, int index);
int64_t parse_get_int(parse_test_context* context, int index);
double parse_get_float(parse_test_context* context, int index);
bool parse_get_bool(parse_test_context* context, int index);
void parse_add_type(parse_test_context* context, parsed_type type);
void parse_add_string(parse_test_context* context, parsed_type type, const char* value);
void parse_add_int(parse_test_context* context, int64_t value);
void parse_add_float(parse_test_context* context, double value);
void parse_add_bool(parse_test_context* context, bool value);

cte_parse_callbacks parse_new_callbacks();


#ifdef __cplusplus 
}
#endif
#endif // parse_test_helpers_H
