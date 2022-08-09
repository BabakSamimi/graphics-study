#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include "..\defines.h"

typedef struct {
    u32 renderer_id;
} VertexBuffer;

VertexBuffer GenVertBuf(void *data, u32 size);

void BindVertBuf(VertexBuffer buf);
void UnBindVertBuf(void);

#endif
