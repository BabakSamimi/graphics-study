#ifndef MEMORY_H
#define MEMORY_H

// If we ever need to call into platform specific memory allocators
#ifndef ALLOC_MEM

#include <stdlib.h>
#define ALLOC_MEM(x) malloc((x))

#endif

#define KB(x) (1024 * (x))
#define MB(x) (1024 * KB((x)))
#define GB(x) (1024 * MB((x)))

#define IS_POWER_OF_2(x) ( ( (x) & ((x)-1) ) == 0)

typedef struct {
    unsigned char   *buffer;
    size_t          buffer_size; 
    size_t          used;
} ArenaMemory;

void InitArena(ArenaMemory *region, void *buffer, size_t size);
void *ArenaAlloc16(ArenaMemory *region, size_t slice_size);
void ResetArena(ArenaMemory *region);

#endif
