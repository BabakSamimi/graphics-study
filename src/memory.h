#ifndef MEMORY_H
#define MEMORY_H

// If we ever need to call into platform specific memory allocators
#ifndef ALLOC_MEM

#include <stdlib.h>
#define ALLOC_MEM(x) malloc(x)

#endif

#define KB(x) ((x)*1024)
#define MB(x) ((x)*KB(1024))
#define GB(x) ((x)*MB(1024))

#define IS_POWER_OF_2(x) ( ( (x) & ((x)-1) ) == 0)

typedef struct {
    unsigned char   *buffer;
    size_t          buffer_size; 
    size_t          used;
} MemoryRegion;

void InitRegion(MemoryRegion *region, void* buffer, size_t size);
void *SliceRegion16(MemoryRegion *region, size_t slice_size);

#endif
