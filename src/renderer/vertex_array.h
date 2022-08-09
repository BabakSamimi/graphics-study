#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include "..\defines.h"

typedef struct {
    u32 count;
    u32 type;
    u32 normalized;
} VertexAttribute;

typedef struct {
    VertexAttribute* attributes;
    u32 count; // attribute count
    u32 stride;
} VertexLayout;

typedef struct {
    u32 renderer_id;
    VertexLayout layout; // Is it neccessary to store it?
} VertexArray;

VertexArray GenVertArr();

void VertLayoutPush(VertexLayout *layout,
                    u32 count,                    
                    u32 type,
                    u32 normalized);

void VABindLayout(VertexArray *va,
                  VertexLayout vl);

void BindVertArr(VertexArray buf);
void UnbindVertArr(void);

#endif
