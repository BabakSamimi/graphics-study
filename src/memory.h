#ifndef MEMORY_H
#define MEMORY_H

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
