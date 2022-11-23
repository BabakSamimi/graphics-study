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

// Id: opengl texture handle
// hash: the hash of the relative path
typedef struct {
    u32 id;
    u64 hash;
} DiffuseTexture;

typedef struct {
    u32 id;
    u64 hash;    
} SpecularTexture;

typedef struct {
    u32 id;
    u64 hash;       
} AmbientTexture;

typedef struct {
    vec3 diffuse, specular, ambient;
    
    DiffuseTexture diffuse_map;
    SpecularTexture specular_map;
    AmbientTexture ambient_map;
    
    float shininess;
} Material;

typedef struct {
    //Vertex  *vertices; // We don't need to store vertices on the CPU side after we upload it to the GPU?
    u32 *indices;
    
    u32 vertex_count, index_count;
    
    Material material;

    VertexArray va;
} Mesh;

#define MAX_HASHES 64
typedef struct {
    u64 hash[MAX_HASHES]; // Hash (key)
    u32 index[MAX_HASHES]; // Index to texture (value)

    u32 count;
} TextureHashes;

typedef struct {
    Mesh *meshes;
    u32 mesh_count;
    
    u8 model_folder_path[512];
} Model;

Model LoadModelFromAssimp(ArenaMemory *memory, ArenaMemory *scratch, u8 *model_folder, u8 *model_name);

#endif
