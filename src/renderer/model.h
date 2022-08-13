#ifndef MODEL_H
#define MODEL_H

#include "vertex_array.h"
#include "..\memory.h"
#include "..\gfx_math.h"
#include "..\defines.h"

typedef struct {
    vec3 position;
    vec3 normal;
    vec2 tex_coords;    
} Vertex;

typedef struct {
    u32 id;
    u8 *type;
    u8 *path;
} Texture;

typedef struct {
    vec3 diffuse, specular, ambient;
    float shininess;
} Material;

typedef struct {
    Vertex  *vertices;
    u32     *indices;
    
    Texture *textures;
    u32 vertex_count, index_count, texture_count;
    Material material;

    VertexArray va;
} Mesh;

typedef struct {
    Mesh *meshes;
    u32 mesh_count;
    u8 *path;
} Model;

Model LoadModelFromAssimp(MemoryRegion memory, u8 *relative_path);

#endif
