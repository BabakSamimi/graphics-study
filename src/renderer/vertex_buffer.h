#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

typedef struct {
    unsigned int renderer_id;
} VertexBuffer;

VertexBuffer GenVertBuf(void* data, unsigned int size);

void BindVertBuf(VertexBuffer buf);
void UnBindVertBuf(void);

#endif
