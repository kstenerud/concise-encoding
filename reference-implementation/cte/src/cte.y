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
    bool bool_v;
}

%type <string_v>  TOKEN_STRING TOKEN_UNEXPECTED TOKEN_BAD_DATA string
%type <int64_v>   TOKEN_INTEGER
%type <float64_v> TOKEN_FLOAT
%type <bool_v>    TOKEN_BOOLEAN

%token TOKEN_BOOLEAN TOKEN_INTEGER TOKEN_FLOAT TOKEN_DECIMAL TOKEN_STRING TOKEN_EMPTY TOKEN_UNEXPECTED TOKEN_BAD_DATA
%token TOKEN_MAP_START TOKEN_MAP_END TOKEN_LIST_START TOKEN_LIST_END TOKEN_ITEM_SEPARATOR TOKEN_ASSIGNMENT_SEPARATOR

%start object

%%

object:
      string           { callbacks->on_string(context, $1); }
    | TOKEN_INTEGER    { callbacks->on_int(context, $1); }
    | TOKEN_FLOAT      { callbacks->on_float(context, $1); }
    | TOKEN_BOOLEAN    { callbacks->on_boolean(context, $1); }
    | TOKEN_EMPTY      { callbacks->on_empty(context); }
    | list
    | map
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

list:  list_start list_entries list_end

list_start: TOKEN_LIST_START { callbacks->on_list_start(context); }

list_end: TOKEN_LIST_END { callbacks->on_list_end(context); }

list_entries: /* empty */
    | object
    | list_entries TOKEN_ITEM_SEPARATOR object

map: map_start map_entries map_end

map_start: TOKEN_MAP_START { callbacks->on_map_start(context); }

map_end: TOKEN_MAP_END { callbacks->on_map_end(context); }

map_tuple: object TOKEN_ASSIGNMENT_SEPARATOR object

map_entries: /* empty */
    | map_tuple
    | map_entries TOKEN_ITEM_SEPARATOR map_tuple

%%

int yylex(YYSTYPE *, void *);
