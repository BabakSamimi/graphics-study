#ifndef INDEX_BUFFER_H
#define INDEX_BUFFER_H

#include "..\defines.h"

typedef struct {
    u32 renderer_id;
    u32 index_count;
} IndexBuffer;

IndexBuffer GenIndexBuf(u32 *indices, u32 index_count);

void BindIndexBuf(IndexBuffer buf);
void UnBindIndexBuf(void);

#endif
