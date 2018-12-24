#pragma once
#ifdef __cplusplus 
extern "C" {
#endif

#include "cte/cte.h"


// Callbacks from parser to decoder.
typedef struct
{
    void (*on_error)             (struct cte_decode_process* decode_process, const char* message);
    bool (*on_empty)             (struct cte_decode_process* decode_process);
    bool (*on_true)              (struct cte_decode_process* decode_process);
    bool (*on_false)             (struct cte_decode_process* decode_process);
    bool (*on_int)               (struct cte_decode_process* decode_process, int base, char* value);
    bool (*on_float)             (struct cte_decode_process* decode_process, char* value);
    bool (*on_decimal)           (struct cte_decode_process* decode_process, char* value);
    bool (*on_time)              (struct cte_decode_process* decode_process, int day_digits, char* value);
    bool (*on_list_begin)        (struct cte_decode_process* decode_process);
    bool (*on_list_end)          (struct cte_decode_process* decode_process);
    bool (*on_map_begin)         (struct cte_decode_process* decode_process);
    bool (*on_map_end)           (struct cte_decode_process* decode_process);
    bool (*on_string)            (struct cte_decode_process* decode_process, char* value);
    bool (*on_binary_data_begin) (struct cte_decode_process* decode_process);
    bool (*on_binary_data_end)   (struct cte_decode_process* decode_process);
} internal_parse_callbacks;


struct cte_decode_process
{
    internal_parse_callbacks parse_callbacks;
    const cte_decode_callbacks* callbacks;
    int document_depth;
    void* user_context;
    void* scanner;
};
typedef struct cte_decode_process cte_decode_process;


// =========================================
// Declarations needed by the lexer & parser
// =========================================

void yyerror(const void* const scanner, struct cte_decode_process* decode_process, const char *msg);

extern internal_parse_callbacks g_cte_parse_callbacks;


#ifdef __cplusplus 
}
#endif
