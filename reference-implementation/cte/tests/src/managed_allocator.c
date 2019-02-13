#include "managed_allocator.h"
#include <stdlib.h>
#include <string.h>

static void* g_allocated[100000];
static int g_allocated_index = 0;

void* managed_allocate(int size)
{
    void* memory = malloc(size);
    g_allocated[g_allocated_index++] = memory;
    return memory;
}

void managed_free_all()
{
    for(int i = 0; i < g_allocated_index; i++)
    {
        free(g_allocated[i]);
    }
    memset(g_allocated, 0, sizeof(g_allocated));
    g_allocated_index = 0;
}
