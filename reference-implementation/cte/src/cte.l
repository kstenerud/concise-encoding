%{

#include "cte/cte.h"
#include "parser.h"
#include <limits.h>
#include <math.h>

// Unquotes and unescapes a string in-place (modifies the original string)
// Returns NULL if successful, or else a pointer to the offending escape sequence.
static char* string_unquote_unescape(char* str);

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

COMMENT      #.*$
WHITESPACE   [ \t\r\n]
STRING_CHAR  [^"\\]|\\["\\/bfnrt]|\\u[0-9A-Fa-f]{4}
TYPE_STRING  \"{STRING_CHAR}*\"
TYPE_INT_2   [-+]?[0-7]+b
TYPE_INT_8   [-+]?[0-7]+o
TYPE_INT_10  [-+]?[0-9]+
TYPE_INT_16  [-+]?[0-9a-f]+h
TYPE_FLOAT   [-+]?[0-9]*\.?[0-9]*([e][-+]?[0-9]+)?
TYPE_DECIMAL [-+]?[0-9]*\.?[0-9]*([e][-+]?[0-9]+)?d
TYPE_TIME    -?[0-9]{4,}-[0-9]{2}-[0-9]{2}T[0-9]{2}:[0-9]{2}:[0-9]{2}(\.[0-9]*)?Z|([-+][0-9]{2}:[0-9]{2})
TYPE_TIME_D  -?[0-9]{4,}-[0-9]{3}T[0-9]{2}:[0-9]{2}:[0-9]{2}(\.[0-9]*)?Z|([-+][0-9]{2}:[0-9]{2})
VALUE_EMPTY  empty
VALUE_TRUE   t|true
VALUE_FALSE  f|false

%%

"{" { return TOKEN_MAP_START; }
"}" { return TOKEN_MAP_END; }
"[" { return TOKEN_LIST_START; }
"]" { return TOKEN_LIST_END; }
":" { return TOKEN_ASSIGNMENT_SEPARATOR; }

"b("    { return TOKEN_ARRAY_BOOLEAN_START; }
"i8("   { return TOKEN_ARRAY_INT_8_START; }
"i16("  { return TOKEN_ARRAY_INT_16_START; }
"i32("  { return TOKEN_ARRAY_INT_32_START; }
"i64("  { return TOKEN_ARRAY_INT_64_START; }
"i128(" { return TOKEN_ARRAY_INT_128_START; }
"f32("  { return TOKEN_ARRAY_FLOAT_32_START; }
"f64("  { return TOKEN_ARRAY_FLOAT_64_START; }
"f128(" { return TOKEN_ARRAY_FLOAT_128_START; }
"d32("  { return TOKEN_ARRAY_DECIMAL_32_START; }
"d64("  { return TOKEN_ARRAY_DECIMAL_64_START; }
"d128(" { return TOKEN_ARRAY_DECIMAL_128_START; }
"t("    { return TOKEN_ARRAY_TIME_START; }
")"     { return TOKEN_ARRAY_END; }

{COMMENT}  {/* Ignored */}
{WHITESPACE}  {/* Ignored */}
{VALUE_EMPTY} { return TOKEN_EMPTY; }
{VALUE_TRUE}  { yylval->bool_v = true; return TOKEN_BOOLEAN; }
{VALUE_FALSE} { yylval->bool_v = false; return TOKEN_BOOLEAN; }

{TYPE_INT_2} {
    // TODO: parse 128 bit integer
    int64_t value = strtoll(yytext, NULL, 2);
    if((value == LLONG_MAX || value == LLONG_MIN) && errno == ERANGE)
    {
        yylval->string_v = yytext;
        return TOKEN_BAD_DATA;
    }
    yylval->int128_v = value;
    return TOKEN_INTEGER;
}

{TYPE_INT_8} {
    // TODO: parse 128 bit integer
    int64_t value = strtoll(yytext, NULL, 8);
    if((value == LLONG_MAX || value == LLONG_MIN) && errno == ERANGE)
    {
        yylval->string_v = yytext;
        return TOKEN_BAD_DATA;
    }
    yylval->int128_v = value;
    return TOKEN_INTEGER;
}

{TYPE_INT_10} {
    // TODO: parse 128 bit integer
    int64_t value = strtoll(yytext, NULL, 10);
    if((value == LLONG_MAX || value == LLONG_MIN) && errno == ERANGE)
    {
        yylval->string_v = yytext;
        return TOKEN_BAD_DATA;
    }
    yylval->int128_v = value;
    return TOKEN_INTEGER;
}

{TYPE_INT_16} {
    // TODO: parse 128 bit integer
    int64_t value = strtoll(yytext, NULL, 16);
    if((value == LLONG_MAX || value == LLONG_MIN) && errno == ERANGE)
    {
        yylval->string_v = yytext;
        return TOKEN_BAD_DATA;
    }
    yylval->int128_v = value;
    return TOKEN_INTEGER;
}

{TYPE_FLOAT} {
    // TODO: parse 128 bit float
	double value = yylval->float128_v = strtod(yytext, NULL);
    if((value == HUGE_VAL || value == -HUGE_VAL) && errno == ERANGE)
    {
        yylval->string_v = yytext;
        return TOKEN_BAD_DATA;
    }
    yylval->float128_v = value;
	return TOKEN_FLOAT;
}

{TYPE_DECIMAL} {
    // TODO
    yylval->decimal128_v = 0;
    return TOKEN_FLOAT;
}

{TYPE_TIME} {
    yylval->string_v = yytext;
    return TOKEN_TIME;
}

{TYPE_TIME_D} {
    yylval->string_v = yytext;
    return TOKEN_TIME;
}

{TYPE_STRING} {
    char* bad_data_loc = string_unquote_unescape(yytext);
    if(bad_data_loc != NULL)
    {
        yylval->string_v = bad_data_loc;
        return TOKEN_BAD_DATA;
    }
    yylval->string_v = yytext;
    return TOKEN_STRING;
}

. {
    yylval->string_v = yytext;
   	return TOKEN_UNEXPECTED;
}

%%

void yyerror (const void* const scanner, const cte_parse_callbacks* const callbacks, void* context, const char* const msg)
{
    (void)scanner;
	callbacks->on_error(context, msg);
}

bool cte_parse_string(const char* const input, const cte_parse_callbacks* const callbacks, void* context)
{
    yyscan_t scanner;

    if(yylex_init(&scanner) != 0)
    {
    	callbacks->on_error(context, "Could not init scanner");
    	return false;
    }

    yy_scan_string(input, scanner);
    bool result = yyparse(scanner, callbacks, context) == 0;
    yylex_destroy(scanner);

    return result;
}

static bool is_hex_character(int ch)
{
    if(ch >= '0' && ch <= '9')
    {
        return true;
    }
    return ch >= 'a' && ch <= 'f';
}

static bool is_octal_character(int ch)
{
    return ch >= '0' && ch <= '7';
}

static char* string_unquote_unescape(char* str)
{
    char* read_pos = str;
    char* read_end = read_pos + strlen(read_pos);
    char* write_pos = str;

    if(*read_pos != '"')
    {
        return read_pos;
    }
    read_pos++;

    for(; read_pos < read_end; read_pos++)
    {
        switch(*read_pos)
        {
            case '"':
                *write_pos = 0;
                return NULL;
            case '\\':
            {
                char* escape_pos = read_pos;
                int remaining_bytes = read_end - read_pos;
                if(remaining_bytes < 1)
                {
                    return escape_pos;
                }

                read_pos++;
                switch(*read_pos)
                {
                    case 'r': *write_pos++ = '\r'; break;
                    case 'n': *write_pos++ = '\n'; break;
                    case 't': *write_pos++ = '\t'; break;
                    case '\\': *write_pos++ = '\\'; break;
                    case '\"': *write_pos++ = '\"'; break;
                    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
                    {
                        uint8_t number_buffer[4] = {*read_pos, 0, 0, 0};
                        read_pos++;
                        if(is_octal_character(*read_pos))
                        {
                            number_buffer[1] = *read_pos;
                            read_pos++;
                        }
                        if(is_octal_character(*read_pos))
                        {
                            number_buffer[2] = *read_pos;
                            read_pos++;
                        }
                        *write_pos++ = (uint8_t)strtoul((char*)number_buffer, NULL, 8);
                        read_pos--;
                        break;
                    }
                    case 'x':
                    {
                        if(remaining_bytes < 2)
                        {
                            return escape_pos;
                        }
                        read_pos++;
                        if(!is_hex_character(*read_pos))
                        {
                            return escape_pos;
                        }
                        uint8_t number_buffer[3] = {*read_pos, 0, 0};
                        read_pos += 1;
                        if(is_hex_character(*read_pos))
                        {
                            number_buffer[1] = *read_pos;
                            read_pos += 1;
                        }
                        *write_pos++ = (uint8_t)strtoul((char*)number_buffer, NULL, 16);
                        read_pos--;
                        break;
                    }
                    case 'u':
                    {
                        if(remaining_bytes < 5)
                        {
                            return escape_pos;
                        }
                        if(!is_hex_character(read_pos[1])
                        || !is_hex_character(read_pos[2])
                        || !is_hex_character(read_pos[3])
                        || !is_hex_character(read_pos[4]))
                        {
                            return escape_pos;
                        }
                        uint8_t number_buffer[5] = {read_pos[1], read_pos[2], read_pos[3], read_pos[4], 0};
                        read_pos += 4;
                        unsigned int codepoint = strtoul((char*)number_buffer, NULL, 16);
                        if(codepoint <= 0x7f)
                        {
                            *write_pos++ = (uint8_t)codepoint;
                        }
                        else if(codepoint <= 0x7ff)
                        {
                            *write_pos++ = (uint8_t)((codepoint >> 6) | 0xc0);
                            *write_pos++ = (uint8_t)((codepoint & 0x3f) | 0x80);
                        }
                        else
                        {
                            *write_pos++ = (uint8_t)((codepoint >> 12) | 0xe0);
                            *write_pos++ = (uint8_t)(((codepoint >> 6) & 0x3f) | 0x80);
                            *write_pos++ = (uint8_t)((codepoint & 0x3f) | 0x80);
                        }
                        break;
                    }
                    default:
                        return escape_pos;
                }
                break;
            }
            default:
                *write_pos++ = *read_pos;
                break;
        }
    }
    *write_pos = 0;
    return NULL;
}
