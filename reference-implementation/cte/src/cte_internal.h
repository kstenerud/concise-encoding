#pragma once
#ifdef __cplusplus 
extern "C" {
#endif

#include "cte/cte.h"

/**
 * Holds encoder contextual data.
 */
typedef struct
{
    const uint8_t* start;
    const uint8_t* end;
    uint8_t* pos;
    int indent_spaces;
    int float_digits_precision;
    int container_level;
    bool is_inside_map[200];
    bool is_first_in_document;
    bool is_first_in_container;
    bool next_object_is_map_key;
    // TODO: allowed_types
} cte_real_encode_process;


typedef struct
{
    cte_decode_callbacks* callbacks;
    int document_depth;
    void* user_context;
    void* scanner;
} cte_real_decode_process;

void yyerror(const void* const scanner, const cte_real_decode_process* decode_process, const char *msg);


#ifdef __cplusplus 
}
#endif
