#include "renderer.h"
#include "index_buffer.h"

IndexBuffer GenIndexBuf(u32 *indices, u32 size)
{
    IndexBuffer buf;
    buf.index_count = size / sizeof(u32);
    glGenBuffers(1, &buf.renderer_id);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.renderer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return buf;
}

void BindIndBuf(IndexBuffer buf)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.renderer_id);
}

void UnbindIndBuf(void)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
