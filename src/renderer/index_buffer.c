#include "index_buffer.h"
#include "glad/glad.h"

IndexBuffer GenIndexBuf(u32 *indices, u32 index_count)
{
    IndexBuffer buf;
    buf.index_count = index_count;
    glGenBuffers(1, &buf.renderer_id);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.renderer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return buf;
}

void BindIndBuf(IndexBuffer buf)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.renderer_id);
}

void UnBindIndBuf(void)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
