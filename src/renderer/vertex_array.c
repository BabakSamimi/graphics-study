#include "renderer.h"
#include "vertex_array.h"

VertexArray GenVertArr()
{
    VertexArray result;
    
    GLuint VAO;
    glGenVertexArrays(1, &VAO);    
    glBindVertexArray(VAO);

    result.renderer_id = VAO;
    
    return result;
}

// Vertex Array operations

// Push an attribute to the layout
void VertLayoutPush(VertexLayout *layout,
                    u32 count,                    
                    u32 type,
                    u32 normalized)
{
    VertexAttribute *attrib = &layout->attributes[layout->count];
    
    attrib->type = type;
    attrib->count = count;    
    attrib->normalized = normalized;   
    
    switch(type)
    {
        case GL_FLOAT:
            layout->stride += count * sizeof(f32);
            break;
            
        case GL_UNSIGNED_INT:
            layout->stride += count * sizeof(u32);
            break;

        case GL_UNSIGNED_BYTE:
            layout->stride += count * sizeof(u8);
            break;
            
        default:
           glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0,                       
                     GL_DEBUG_SEVERITY_HIGH, -1, "Tried to push unsupported GL Type to a vertex layout");
    }
    
    layout->count++;
}

// Associate a vertex array with a vertex layout
void VABindLayout(VertexArray *va,
                  VertexLayout vl)
{
    va->layout = vl;
    size_t offset = 0;
    
    for(u32 attrib_idx = 0; attrib_idx < vl.count; attrib_idx++)
    {
        VertexAttribute attribute = vl.attributes[attrib_idx];
        
        
        glVertexAttribPointer(attrib_idx, attribute.count, attribute.type, attribute.normalized, vl.stride, (void*)offset);
        
        glEnableVertexAttribArray(attrib_idx);        

        switch(attribute.type)
        {
            case GL_FLOAT:
                offset += attribute.count * sizeof(f32);
                break;
            
            case GL_UNSIGNED_INT:
                offset += attribute.count * sizeof(u32);
                break;

            case GL_UNSIGNED_BYTE:
                offset += attribute.count * sizeof(u8);
                break;
            
            default:
                glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0,                       
                                     GL_DEBUG_SEVERITY_HIGH, -1, "Tried to bind a Vertex Layout with an unsupported attribute type!");
        }        

    }

}

void BindVertArr(VertexArray buf)
{
    glBindVertexArray(buf.renderer_id);
}

void UnbindVertArr(void)
{
    glBindVertexArray(0);
}
