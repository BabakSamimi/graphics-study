#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include "vertex_array.h"
#include "camera.h"
#include "..\memory.h"

typedef struct {
    VertexArray cube_va, ui_va;
    MemoryRegion global_mesh_memory;
    Camera cam;
} RenderManager;

#endif
