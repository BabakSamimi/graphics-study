#include <stdint.h>
#include <assert.h>
#include <memory.h> // memset

#include "memory.h"

/*
  Different types of memories:

  Permament: Freed at the end of the program
  Transient: Cycle-based lifetime
  Scratch: Very short-lived

  TODO: Implement these purpose-specific memory types
  
*/

void InitRegion(MemoryRegion *region, void* backing_buffer, size_t size)
{
    
    region->buffer = (unsigned char*) backing_buffer;
    region->buffer_size = size;
    region->used = 0;

    memset(region->buffer, 0, region->buffer_size);
    
}

// 16-byte aligned memory region
void *SliceRegion16(MemoryRegion *region, size_t slice_size)
{

    void *result;
    
    size_t curr_ptr = (size_t)region->buffer + region->used;
    size_t alignment_offset = 0;
    
    // Check that the curr_ptr is 16-byte aligned
    if(curr_ptr & 15)
    {
        alignment_offset = 16 - (curr_ptr & 15);
    }

    region->used += alignment_offset;        

    assert((region->used + slice_size) <= region->buffer_size);

    region->used += slice_size;    
    result = (void*)(curr_ptr + alignment_offset);
    
    return result;
    
}
