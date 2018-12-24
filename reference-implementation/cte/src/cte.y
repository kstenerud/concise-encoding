%{

#include <stdio.h>
#include "decoder.h"
#include "parser.h"

#if YYBISON
union YYSTYPE;
int yylex(union YYSTYPE *, void *);
#endif

#define CALL_HANDLER(FUNCTION_NAME, ...) \
{ \
    if(!process->parse_callbacks.FUNCTION_NAME(__VA_ARGS__)) \
    { \
        return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; \
    } \
}

%}

%define api.pure full
%define parse.error verbose

%lex-param { void *scanner }

%parse-param { void *scanner }
%parse-param { cte_decode_process* process }

%union {
    char* string_v;
}

%type <string_v> TOKEN_INTEGER_2 TOKEN_INTEGER_8 TOKEN_INTEGER_10 TOKEN_INTEGER_16 TOKEN_FLOAT TOKEN_DECIMAL TOKEN_TIME_DD TOKEN_TIME_DDD TOKEN_STRING TOKEN_UNEXPECTED TOKEN_BAD_DATA

%token TOKEN_TRUE TOKEN_FALSE TOKEN_FLOAT TOKEN_DECIMAL TOKEN_STRING TOKEN_TIME_DD TOKEN_TIME_DDD TOKEN_EMPTY TOKEN_UNEXPECTED TOKEN_BAD_DATA
%token TOKEN_INTEGER_2 TOKEN_INTEGER_8 TOKEN_INTEGER_10 TOKEN_INTEGER_16
%token TOKEN_MAP_START TOKEN_MAP_END TOKEN_LIST_START TOKEN_LIST_END TOKEN_ASSIGNMENT_SEPARATOR
%token TOKEN_BINARY_DATA_START TOKEN_BINARY_DATA_END

%start object

%%

array_object:
      TOKEN_TRUE       { CALL_HANDLER(on_true,    process); }
    | TOKEN_FALSE      { CALL_HANDLER(on_false,   process); }
    | TOKEN_INTEGER_2  { CALL_HANDLER(on_int,     process, 2, $1); }
    | TOKEN_INTEGER_8  { CALL_HANDLER(on_int,     process, 8, $1); }
    | TOKEN_INTEGER_10 { CALL_HANDLER(on_int,     process, 10, $1); }
    | TOKEN_INTEGER_16 { CALL_HANDLER(on_int,     process, 16, $1); }
    | TOKEN_FLOAT      { CALL_HANDLER(on_float,   process, $1); }
    | TOKEN_DECIMAL    { CALL_HANDLER(on_decimal, process, $1); }
    | TOKEN_TIME_DD    { CALL_HANDLER(on_time,    process, 2, $1); }
    | TOKEN_TIME_DDD   { CALL_HANDLER(on_time,    process, 3, $1); }

object:
      array_object
    | TOKEN_STRING     { CALL_HANDLER(on_string,  process, $1); }
    | TOKEN_EMPTY      { CALL_HANDLER(on_empty,   process); }
    | list
    | map
    | array
    | TOKEN_UNEXPECTED {
        char buff[1000];
        snprintf(buff, sizeof(buff), "Unexpected token: %s", $1);
        buff[sizeof(buff)-1] = 0;
        process->parse_callbacks.on_error(process, buff);
        return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK;
    }
    | TOKEN_BAD_DATA {
        char buff[1000];
        snprintf(buff, sizeof(buff), "Bad encoding: %s", $1);
        buff[sizeof(buff)-1] = 0;
        process->parse_callbacks.on_error(process, buff);
        return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK;
    }

array:         array_start array_entries array_end
array_start:   TOKEN_BINARY_DATA_START { CALL_HANDLER(on_binary_data_begin, process); }
array_end:     TOKEN_BINARY_DATA_END   { CALL_HANDLER(on_binary_data_end, process); }
array_entries: /* empty */ | array_object | array_entries array_object

list:          list_start list_entries list_end
list_start:    TOKEN_LIST_START        { CALL_HANDLER(on_list_begin, process); }
list_end:      TOKEN_LIST_END          { CALL_HANDLER(on_list_end, process); }
list_entries:  /* empty */ | object | list_entries object

map:           map_start map_entries map_end
map_start:     TOKEN_MAP_START         { CALL_HANDLER(on_map_begin, process); }
map_end:       TOKEN_MAP_END           { CALL_HANDLER(on_map_end, process); }
map_tuple:     object TOKEN_ASSIGNMENT_SEPARATOR object
map_entries:   /* empty */ | map_tuple | map_entries map_tuple

%%

int yylex(YYSTYPE *, void *);
