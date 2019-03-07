#ifndef cte_internal_H
#define cte_internal_H

#include "cte/cte.h"

typedef enum
{
    ARRAY_TYPE_STRING,
    ARRAY_TYPE_BINARY,
    ARRAY_TYPE_COMMENT,
} array_type;

static inline int get_max_container_depth_or_default(int max_container_depth)
{
    return max_container_depth > 0 ? max_container_depth : CTE_DEFAULT_MAX_CONTAINER_DEPTH;
}

static inline void zero_memory(void* const memory, const int byte_count)
{
    uint8_t* ptr = memory;
    uint8_t* const end = ptr + byte_count;
    while(ptr < end)
    {
        *ptr++ = 0;
    }
}

bool cte_validate_string(const uint8_t* const start, const int64_t byte_count);

bool cte_validate_comment(const uint8_t* const start, const int64_t byte_count);

#endif // cte_internal_H
