#ifndef MODEL_H
#define MODEL_H

#include <stdbool.h>

#include "vertex_array.h"
#include "..\memory.h"
#include "..\gfx_math.h"
#include "..\defines.h"

typedef struct {
    vec3 position;
    vec3 normal;
    vec2 tex_coords;    
} Vertex;

typedef enum {
    AMBIENT, DIFFUSE, SPECULAR
} TexEnum;

typedef struct {
    u32 id;
    TexEnum type;
} Texture;

typedef struct {
    u32 id;
} DiffuseTexture;

typedef struct {
    u32 id;
} SpecularTexture;

typedef struct {
    vec3 diffuse, specular, ambient;
    
    DiffuseTexture diffuse_map;
    SpecularTexture specular_map;
    
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
    u8 model_folder_path[512];
} Model;

Model LoadModelFromAssimp(MemoryRegion memory, u8 *model_folder, u8 *model_name);

#endif
