%{

#include "cte_internal.h"
#include "parser.h"
#include <string.h>
#include <stdio.h>

#if YYBISON
union YYSTYPE;
int yylex(union YYSTYPE *, void *);
#endif


%}

%define api.pure full
%define parse.error verbose

%lex-param { void *scanner }

%parse-param { void *scanner }
%parse-param { cte_real_decode_process* process }

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
      TOKEN_BOOLEAN    { process->callbacks->on_boolean((cte_decode_process*)process, $1); }
    | TOKEN_INTEGER    { process->callbacks->on_int_64((cte_decode_process*)process, $1); }
    | TOKEN_FLOAT      { process->callbacks->on_float_64((cte_decode_process*)process, $1); }
    | TOKEN_DECIMAL    { process->callbacks->on_decimal_64((cte_decode_process*)process, $1); }
    | TOKEN_TIME       { process->callbacks->on_time((cte_decode_process*)process, $1); }

object:
      array_object
    | string           { process->callbacks->on_string((cte_decode_process*)process, $1); }
    | TOKEN_EMPTY      { process->callbacks->on_empty((cte_decode_process*)process); }
    | list
    | map
    | array
    | TOKEN_UNEXPECTED {
        char buff[1000];
        snprintf(buff, sizeof(buff), "Unexpected token: %s", $1);
        buff[sizeof(buff)-1] = 0;
        process->callbacks->on_error((cte_decode_process*)process, buff);
        return -1;
    }
    | TOKEN_BAD_DATA {
        char buff[1000];
        snprintf(buff, sizeof(buff), "Bad encoding: %s", $1);
        buff[sizeof(buff)-1] = 0;
        process->callbacks->on_error((cte_decode_process*)process, buff);
        return -1;
    }

string: TOKEN_STRING

array:  array_start array_entries array_end

array_start:
      TOKEN_ARRAY_BOOLEAN_START     { process->callbacks->on_array_begin_boolean((cte_decode_process*)process); }
    | TOKEN_ARRAY_INT_8_START       { process->callbacks->on_array_begin_int_8((cte_decode_process*)process); }
    | TOKEN_ARRAY_INT_16_START      { process->callbacks->on_array_begin_int_16((cte_decode_process*)process); }
    | TOKEN_ARRAY_INT_32_START      { process->callbacks->on_array_begin_int_32((cte_decode_process*)process); }
    | TOKEN_ARRAY_INT_64_START      { process->callbacks->on_array_begin_int_64((cte_decode_process*)process); }
    | TOKEN_ARRAY_INT_128_START     { process->callbacks->on_array_begin_int_128((cte_decode_process*)process); }
    | TOKEN_ARRAY_FLOAT_32_START    { process->callbacks->on_array_begin_float_32((cte_decode_process*)process); }
    | TOKEN_ARRAY_FLOAT_64_START    { process->callbacks->on_array_begin_float_64((cte_decode_process*)process); }
    | TOKEN_ARRAY_FLOAT_128_START   { process->callbacks->on_array_begin_float_128((cte_decode_process*)process); }
    | TOKEN_ARRAY_DECIMAL_32_START  { process->callbacks->on_array_begin_decimal_32((cte_decode_process*)process); }
    | TOKEN_ARRAY_DECIMAL_64_START  { process->callbacks->on_array_begin_decimal_64((cte_decode_process*)process); }
    | TOKEN_ARRAY_DECIMAL_128_START { process->callbacks->on_array_begin_decimal_128((cte_decode_process*)process); }
    | TOKEN_ARRAY_TIME_START        { process->callbacks->on_array_begin_time((cte_decode_process*)process); }

array_end: TOKEN_ARRAY_END { process->callbacks->on_array_end((cte_decode_process*)process); }

array_entries: /* empty */
    | array_object
    | array_entries array_object

list:  list_start list_entries list_end

list_start: TOKEN_LIST_START { process->callbacks->on_list_begin((cte_decode_process*)process); }

list_end: TOKEN_LIST_END { process->callbacks->on_list_end((cte_decode_process*)process); }

list_entries: /* empty */
    | object
    | list_entries object

map: map_start map_entries map_end

map_start: TOKEN_MAP_START { process->callbacks->on_map_begin((cte_decode_process*)process); }

map_end: TOKEN_MAP_END { process->callbacks->on_map_end((cte_decode_process*)process); }

map_tuple: object TOKEN_ASSIGNMENT_SEPARATOR object

map_entries: /* empty */
    | map_tuple
    | map_entries map_tuple

%%

int yylex(YYSTYPE *, void *);
