#ifndef INDEX_BUFFER_H
#define INDEX_BUFFER_H

typedef struct {
    unsigned int renderer_id;
    unsigned int index_count;
} IndexBuffer;

IndexBuffer GenIndexBuf(unsigned int* indices, unsigned int index_count);

void BindIndexBuf(IndexBuffer buf);
void UnBindIndexBuf(void);

#endif
