#pragma once
#ifdef __cplusplus 
extern "C" {
#endif

#include "cte/cte.h"

typedef enum
{
    // enum value & 0xff == sizeof(type)
    CTE_TYPE_NONE         = 0,
    CTE_TYPE_BOOLEAN_8    = 0x100 + sizeof(bool),
    CTE_TYPE_INT_8        = 0x200 + sizeof(int8_t),
    CTE_TYPE_INT_16       = 0x300 + sizeof(int16_t),
    CTE_TYPE_INT_32       = 0x400 + sizeof(int32_t),
    CTE_TYPE_INT_64       = 0x500 + sizeof(int64_t),
    CTE_TYPE_INT_128      = 0x600 + sizeof(__int128),
    CTE_TYPE_FLOAT_32     = 0x700 + sizeof(float),
    CTE_TYPE_FLOAT_64     = 0x800 + sizeof(double),
    CTE_TYPE_FLOAT_128    = 0x900 + sizeof(__float128),
    CTE_TYPE_DECIMAL_32   = 0xa00 + sizeof(_Decimal32),
    CTE_TYPE_DECIMAL_64   = 0xb00 + sizeof(_Decimal64),
    CTE_TYPE_DECIMAL_128  = 0xc00 + sizeof(_Decimal128),
    CTE_TYPE_TIME_64      = 0xd00 + sizeof(smalltime),
} cte_data_type;

struct cte_real_decode_process;

typedef struct
{
    void (*on_error)       (struct cte_real_decode_process* decode_process, const char* message);
    int  (*on_empty)       (struct cte_real_decode_process* decode_process);
    int  (*on_true)        (struct cte_real_decode_process* decode_process);
    int  (*on_false)       (struct cte_real_decode_process* decode_process);
    int  (*on_int)         (struct cte_real_decode_process* decode_process, int base, const char* value);
    int  (*on_float)       (struct cte_real_decode_process* decode_process, const char* value);
    int  (*on_decimal)     (struct cte_real_decode_process* decode_process, const char* value);
    int  (*on_time)        (struct cte_real_decode_process* decode_process, const char* value);
    int  (*on_list_begin)  (struct cte_real_decode_process* decode_process);
    int  (*on_list_end)    (struct cte_real_decode_process* decode_process);
    int  (*on_map_begin)   (struct cte_real_decode_process* decode_process);
    int  (*on_map_end)     (struct cte_real_decode_process* decode_process);
    int  (*on_string)      (struct cte_real_decode_process* decode_process, const char* value);
    int  (*on_array_begin) (struct cte_real_decode_process* decode_process, cte_data_type type);
    int  (*on_array_end)   (struct cte_real_decode_process* decode_process);
} internal_parse_callbacks;



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


typedef struct cte_real_decode_process
{
    internal_parse_callbacks parse_callbacks;
    cte_decode_callbacks* callbacks;
    int document_depth;
    void* user_context;
    void* scanner;
} cte_real_decode_process;

void yyerror(const void* const scanner, cte_real_decode_process* decode_process, const char *msg);


#ifdef __cplusplus 
}
#endif
