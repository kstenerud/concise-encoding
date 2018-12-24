%{

#include "cte_internal.h"
#include "parser.h"

%}

%option 8bit
%option bison-bridge
  //%option debug
%option noinput
%option nounput
%option noyywrap
%option pointer
%option reentrant
%option warn

COMMENT       #.*$
WHITESPACE    [ \t\r\n]
STRING_CHAR   [^"\\]|\\["\\/bfnrt]|\\u[0-9A-Fa-f]{4}
TYPE_STRING   \"{STRING_CHAR}*\"
TYPE_INT_2    [-+]?[0-7]+b
TYPE_INT_8    [-+]?[0-7]+o
TYPE_INT_10   [-+]?[0-9]+
TYPE_INT_16   [-+]?[0-9a-f]+h
TYPE_FLOAT    [-+]?[0-9]*\.?[0-9]*([e][-+]?[0-9]+)?
TYPE_DECIMAL  [-+]?[0-9]*\.?[0-9]*([e][-+]?[0-9]+)?d
TYPE_TIME_DD  -?[0-9]{4,}-[0-9]{2}-[0-9]{2}T[0-9]{2}:[0-9]{2}:[0-9]{2}(\.[0-9]*)?Z|([-+][0-9]{2}:[0-9]{2})
TYPE_TIME_DDD -?[0-9]{4,}-[0-9]{3}T[0-9]{2}:[0-9]{2}:[0-9]{2}(\.[0-9]*)?Z|([-+][0-9]{2}:[0-9]{2})
VALUE_EMPTY   empty
VALUE_TRUE    t|true
VALUE_FALSE   f|false

%%

"{" { return TOKEN_MAP_START; }
"}" { return TOKEN_MAP_END; }
"[" { return TOKEN_LIST_START; }
"]" { return TOKEN_LIST_END; }
":" { return TOKEN_ASSIGNMENT_SEPARATOR; }

"h("    { return TOKEN_BINARY_DATA_START; }
")"     { return TOKEN_BINARY_DATA_END; }

{COMMENT}       {/* Ignored */}
{WHITESPACE}    {/* Ignored */}
{VALUE_EMPTY}   { return TOKEN_EMPTY; }
{VALUE_TRUE}    { return TOKEN_TRUE; }
{VALUE_FALSE}   { return TOKEN_FALSE; }
{TYPE_INT_2}    { yylval->string_v = yytext; return TOKEN_INTEGER_2; }
{TYPE_INT_8}    { yylval->string_v = yytext; return TOKEN_INTEGER_8; }
{TYPE_INT_10}   { yylval->string_v = yytext; return TOKEN_INTEGER_10; }
{TYPE_INT_16}   { yylval->string_v = yytext; return TOKEN_INTEGER_16; }
{TYPE_FLOAT}    { yylval->string_v = yytext; return TOKEN_FLOAT; }
{TYPE_DECIMAL}  { yylval->string_v = yytext; return TOKEN_DECIMAL; }
{TYPE_TIME_DD}  { yylval->string_v = yytext; return TOKEN_TIME_DD; }
{TYPE_TIME_DDD} { yylval->string_v = yytext; return TOKEN_TIME_DDD; }
{TYPE_STRING}   { yylval->string_v = yytext; return TOKEN_STRING; }

. {
    yylval->string_v = yytext;
   	return TOKEN_UNEXPECTED;
}

%%

void yyerror(const void* const scanner, cte_decode_process* process, const char* msg)
{
    (void)scanner;
	process->parse_callbacks.on_error(process, msg);
}

cte_decode_status cte_decode_feed(cte_decode_process* process,
                                  const char* const data_start,
                                  const int64_t byte_count)
{
    // TODO: Move this to cte_decode_begin
    if(process->scanner == NULL)
    {
        if(yylex_init(&process->scanner) != 0)
        {
            process->callbacks->on_error(process, "Could not initialize scanner");
            return CTE_DECODE_ERROR_PARSER_INITIALIZATION_ERROR;
        }
    }
    yy_scan_bytes((const char* const)data_start, byte_count, process->scanner);
    return yyparse(process->scanner, process);
}

cte_decode_status cte_decode_end(cte_decode_process* process)
{
    yylex_destroy(process->scanner);
    return CTE_DECODE_STATUS_OK;
}

