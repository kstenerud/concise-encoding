%{

#include "cte/cte.h"
#include "parser.h"
#include <string.h>
#include <stdio.h>

#if YYBISON
union YYSTYPE;
int yylex(union YYSTYPE *, void *);
#endif

void yyerror(const void *scanner, const cte_parse_callbacks* callbacks, void* context, const char *msg);

%}

%define api.pure full
%define parse.error verbose

%lex-param { void *scanner }

%parse-param { void *scanner }
%parse-param { const cte_parse_callbacks* callbacks }
%parse-param { void *context }

%union {
    char* string_v;
    __int128_t int128_v;
    __float128 float128_v;
     _Decimal128 decimal128_v;
    bool bool_v;
}

%type <string_v>    TOKEN_TIME TOKEN_STRING TOKEN_UNEXPECTED TOKEN_BAD_DATA string
%type <int128_v>     TOKEN_INTEGER
%type <float128_v>   TOKEN_FLOAT
%type <bool_v>      TOKEN_BOOLEAN
%type <decimal128_v> TOKEN_DECIMAL

%token TOKEN_BOOLEAN TOKEN_INTEGER TOKEN_FLOAT TOKEN_DECIMAL TOKEN_STRING TOKEN_TIME TOKEN_EMPTY TOKEN_UNEXPECTED TOKEN_BAD_DATA
%token TOKEN_MAP_START TOKEN_MAP_END TOKEN_LIST_START TOKEN_LIST_END TOKEN_ASSIGNMENT_SEPARATOR
%token TOKEN_ARRAY_BOOLEAN_START TOKEN_ARRAY_INT_8_START TOKEN_ARRAY_INT_16_START TOKEN_ARRAY_INT_32_START
%token TOKEN_ARRAY_INT_64_START TOKEN_ARRAY_INT_128_START TOKEN_ARRAY_FLOAT_32_START TOKEN_ARRAY_FLOAT_64_START
%token TOKEN_ARRAY_FLOAT_128_START TOKEN_ARRAY_DECIMAL_32_START TOKEN_ARRAY_DECIMAL_64_START TOKEN_ARRAY_DECIMAL_128_START
%token TOKEN_ARRAY_TIME_START TOKEN_ARRAY_END

%start object

%%

array_object:
      TOKEN_BOOLEAN    { callbacks->on_boolean(context, $1); }
    | TOKEN_INTEGER    { callbacks->on_int_64(context, $1); }
    | TOKEN_FLOAT      { callbacks->on_float_64(context, $1); }
    | TOKEN_DECIMAL    { callbacks->on_decimal_64(context, $1); }
    | TOKEN_TIME       { callbacks->on_time(context, $1); }

object:
      array_object
    | string           { callbacks->on_string(context, $1); }
    | TOKEN_EMPTY      { callbacks->on_empty(context); }
    | list
    | map
    | array
    | TOKEN_UNEXPECTED {
        char buff[1000];
        snprintf(buff, sizeof(buff), "Unexpected token: %s", $1);
        buff[sizeof(buff)-1] = 0;
        callbacks->on_error(context, buff);
        return -1;
    }
    | TOKEN_BAD_DATA {
        char buff[1000];
        snprintf(buff, sizeof(buff), "Bad encoding: %s", $1);
        buff[sizeof(buff)-1] = 0;
        callbacks->on_error(context, buff);
        return -1;
    }

string: TOKEN_STRING

array:  array_start array_entries array_end

array_start:
      TOKEN_ARRAY_BOOLEAN_START     { callbacks->on_array_start_boolean(context); }
    | TOKEN_ARRAY_INT_8_START       { callbacks->on_array_start_int_8(context); }
    | TOKEN_ARRAY_INT_16_START      { callbacks->on_array_start_int_16(context); }
    | TOKEN_ARRAY_INT_32_START      { callbacks->on_array_start_int_32(context); }
    | TOKEN_ARRAY_INT_64_START      { callbacks->on_array_start_int_64(context); }
    | TOKEN_ARRAY_INT_128_START     { callbacks->on_array_start_int_128(context); }
    | TOKEN_ARRAY_FLOAT_32_START    { callbacks->on_array_start_float_32(context); }
    | TOKEN_ARRAY_FLOAT_64_START    { callbacks->on_array_start_float_64(context); }
    | TOKEN_ARRAY_FLOAT_128_START   { callbacks->on_array_start_float_128(context); }
    | TOKEN_ARRAY_DECIMAL_32_START  { callbacks->on_array_start_decimal_32(context); }
    | TOKEN_ARRAY_DECIMAL_64_START  { callbacks->on_array_start_decimal_64(context); }
    | TOKEN_ARRAY_DECIMAL_128_START { callbacks->on_array_start_decimal_128(context); }
    | TOKEN_ARRAY_TIME_START        { callbacks->on_array_start_time(context); }

array_end: TOKEN_ARRAY_END { callbacks->on_array_end(context); }

array_entries: /* empty */
    | array_object
    | array_entries array_object

list:  list_start list_entries list_end

list_start: TOKEN_LIST_START { callbacks->on_list_start(context); }

list_end: TOKEN_LIST_END { callbacks->on_list_end(context); }

list_entries: /* empty */
    | object
    | list_entries object

map: map_start map_entries map_end

map_start: TOKEN_MAP_START { callbacks->on_map_start(context); }

map_end: TOKEN_MAP_END { callbacks->on_map_end(context); }

map_tuple: object TOKEN_ASSIGNMENT_SEPARATOR object

map_entries: /* empty */
    | map_tuple
    | map_entries map_tuple

%%

int yylex(YYSTYPE *, void *);
