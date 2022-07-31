#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

typedef struct {
    unsigned int count;
    unsigned int type;
    unsigned int normalized;
} VertexAttribute;

typedef struct {
    VertexAttribute* attributes;
    unsigned int count; // attribute count
    unsigned int stride;
} VertexLayout;

typedef struct {
    unsigned int renderer_id;
    VertexLayout layout; // Is it neccessary to store it?
} VertexArray;

VertexArray GenVertArr();

void VertLayoutPush(VertexLayout *layout,
                    unsigned int count,                    
                    unsigned int type,
                    unsigned int normalized);

void VABindLayout(VertexArray *va,
                  VertexLayout vl);

void BindVertArr(VertexArray buf);
void UnbindVertArr(void);

#endif
