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
    int64_t int64_v;
    double float64_v;
#if CTE_HAS_DECIMAL_SUPPORT
     _Decimal64 decimal64_v;
#else
     float decimal64_v;
#endif
    bool bool_v;
}

%type <string_v>    TOKEN_TIME TOKEN_STRING TOKEN_UNEXPECTED TOKEN_BAD_DATA string
%type <int64_v>     TOKEN_INTEGER
%type <float64_v>   TOKEN_FLOAT
%type <bool_v>      TOKEN_BOOLEAN
%type <decimal64_v> TOKEN_DECIMAL

%token TOKEN_BOOLEAN TOKEN_INTEGER TOKEN_FLOAT TOKEN_DECIMAL TOKEN_STRING TOKEN_TIME TOKEN_EMPTY TOKEN_UNEXPECTED TOKEN_BAD_DATA
%token TOKEN_MAP_START TOKEN_MAP_END TOKEN_LIST_START TOKEN_LIST_END TOKEN_ITEM_SEPARATOR TOKEN_ASSIGNMENT_SEPARATOR
%token TOKEN_ARRAY_BOOLEAN_START TOKEN_ARRAY_INT_8_START TOKEN_ARRAY_INT_16_START TOKEN_ARRAY_INT_32_START
%token TOKEN_ARRAY_INT_64_START TOKEN_ARRAY_INT_128_START TOKEN_ARRAY_FLOAT_32_START TOKEN_ARRAY_FLOAT_64_START
%token TOKEN_ARRAY_FLOAT_128_START TOKEN_ARRAY_DECIMAL_64_START TOKEN_ARRAY_DECIMAL_128_START
%token TOKEN_ARRAY_TIME_START TOKEN_ARRAY_END

%start object

%%

array_object:
      TOKEN_BOOLEAN    { callbacks->on_boolean(context, $1); }
    | TOKEN_INTEGER    { callbacks->on_int(context, $1); }
    | TOKEN_FLOAT      { callbacks->on_float(context, $1); }
    | TOKEN_DECIMAL    {
      #if CTE_HAS_DECIMAL_SUPPORT
          callbacks->on_decimal(context, $1);
      #endif
      }
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
        callbacks->on_parse_error(context, buff);
        return -1;
    }
    | TOKEN_BAD_DATA {
        char buff[1000];
        snprintf(buff, sizeof(buff), "Bad encoding: %s", $1);
        callbacks->on_parse_error(context, buff);
        return -1;
    }

string: TOKEN_STRING {
		char* str = $1;
		str[strlen(str)-1] = 0;
		$$ = str + 1;
	}

array:  array_start array_entries array_end

array_start:
      TOKEN_ARRAY_BOOLEAN_START     { callbacks->on_array_start(context, CTE_TYPE_BOOLEAN,  1); }
    | TOKEN_ARRAY_INT_8_START       { callbacks->on_array_start(context, CTE_TYPE_INTEGER,  1); }
    | TOKEN_ARRAY_INT_16_START      { callbacks->on_array_start(context, CTE_TYPE_INTEGER,  2); }
    | TOKEN_ARRAY_INT_32_START      { callbacks->on_array_start(context, CTE_TYPE_INTEGER,  4); }
    | TOKEN_ARRAY_INT_64_START      { callbacks->on_array_start(context, CTE_TYPE_INTEGER,  8); }
    | TOKEN_ARRAY_INT_128_START     { callbacks->on_array_start(context, CTE_TYPE_INTEGER, 16); }
    | TOKEN_ARRAY_FLOAT_32_START    { callbacks->on_array_start(context, CTE_TYPE_FLOAT,    4); }
    | TOKEN_ARRAY_FLOAT_64_START    { callbacks->on_array_start(context, CTE_TYPE_FLOAT,    8); }
    | TOKEN_ARRAY_FLOAT_128_START   { callbacks->on_array_start(context, CTE_TYPE_FLOAT,   16); }
    | TOKEN_ARRAY_DECIMAL_64_START  { callbacks->on_array_start(context, CTE_TYPE_DECIMAL,  8); }
    | TOKEN_ARRAY_DECIMAL_128_START { callbacks->on_array_start(context, CTE_TYPE_DECIMAL, 16); }
    | TOKEN_ARRAY_TIME_START        { callbacks->on_array_start(context, CTE_TYPE_TIME,     8); }

array_end: TOKEN_ARRAY_END { callbacks->on_array_end(context); }

array_entries: /* empty */
    | array_object
    | array_entries array_object
    | array_entries TOKEN_ITEM_SEPARATOR

list:  list_start list_entries list_end

list_start: TOKEN_LIST_START { callbacks->on_list_start(context); }

list_end: TOKEN_LIST_END { callbacks->on_list_end(context); }

list_entries: /* empty */
    | object
    | list_entries object
    | list_entries TOKEN_ITEM_SEPARATOR

map: map_start map_entries map_end

map_start: TOKEN_MAP_START { callbacks->on_map_start(context); }

map_end: TOKEN_MAP_END { callbacks->on_map_end(context); }

map_tuple: object TOKEN_ASSIGNMENT_SEPARATOR object

map_entries: /* empty */
    | map_tuple
    | map_entries map_tuple
    | map_entries TOKEN_ITEM_SEPARATOR

%%

int yylex(YYSTYPE *, void *);
