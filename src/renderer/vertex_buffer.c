#include "vertex_buffer.h"
#include "glad/glad.h"

VertexBuffer GenVertBuf(void *data, unsigned int size)
{
    VertexBuffer buf;
    glGenBuffers(1, &buf.renderer_id);
    glBindBuffer(GL_ARRAY_BUFFER, buf.renderer_id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return buf;
}

void BindVertBuf(VertexBuffer buf)
{
    glBindBuffer(GL_ARRAY_BUFFER, buf.renderer_id);
}

void UnbindVertBuf(void)
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
