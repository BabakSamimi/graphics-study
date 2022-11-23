#include <stdbool.h>
#include <string.h>

#include "renderer.h"
#include "model.h"
#include "vertex_buffer.h"
#include "index_buffer.h"

#include "assimp/types.h"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/material.h"
#include "assimp/postprocess.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

// 32-bit FNV
#define FNV_OFFSET_BASIS 2166136261
#define FNV_PRIME 16777619

u64 fnv_1a(u8 *data, size_t size)
{
	u64 hash = FNV_OFFSET_BASIS;
	for(u8 index = 0; index < size; index++)
	{
		hash = (hash ^ data[index]) * FNV_PRIME;
	}
	
	return hash;
}

static u32 LoadTexture(u8 *path, s32 flipped)
{
    // Setup Texture
    GLuint texture;
    glGenTextures(1, &texture);

    // Load our texture
    s32 tex_width, tex_height, nr_channels;
    u8 *tex_data = stbi_load(path, &tex_width, &tex_height, &nr_channels, 0);
    
    if(tex_data)
    {
        GLenum format;
        
        if (nr_channels == 3)
            format = GL_RGB;
        else if (nr_channels == 4)
            format = GL_RGBA;

        // Upload to GPU
        glBindTexture(GL_TEXTURE_2D, texture);        
        glTexImage2D(GL_TEXTURE_2D, 0, format, tex_width, tex_height, 0, format, GL_UNSIGNED_BYTE, tex_data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glBindTexture(GL_TEXTURE_2D, 0);    
        stbi_set_flip_vertically_on_load(flipped); // this image starts at top left            
        stbi_image_free(tex_data);
        
        printf("Loaded texture: %s\n", path);
    }
    else printf("Texture was not loaded.\n");
    
    return texture;
}

static Mesh CreateMeshFromAssimp(ArenaMemory *scratch,
                                 char *model_folder_path,
                                 struct aiMesh *mesh,
                                 const struct aiScene *scene)
{
    
    Mesh result = {0};
    
    result.vertex_count = mesh->mNumVertices;
    result.index_count = mesh->mNumFaces * 3; // @Important: A face could be connected by more than 3 vertices, but if we use aiProcess_Triangulate flag when loading with assimp, then we can always be sure that a face is always a triangle.


    //result.vertices = (Vertex*) ArenaAlloc16(&memory, result.vertex_count * sizeof(Vertex));
    Vertex  *vertices = (Vertex*) ArenaAlloc16(scratch, result.vertex_count * sizeof(Vertex));
    result.indices =    (u32*)    ALLOC_MEM(result.index_count * sizeof(u32));

    // Fill the vertex array of the mesh
    for(u32 vertex_index = 0; vertex_index < result.vertex_count; vertex_index++)
    {
        Vertex vertex;
        vec3 v;
        
        v.x = mesh->mVertices[vertex_index].x;
        v.y = mesh->mVertices[vertex_index].y;
        v.z = mesh->mVertices[vertex_index].z;
        vertex.position = v;

        v.x = mesh->mNormals[vertex_index].x;
        v.y = mesh->mNormals[vertex_index].y;
        v.z = mesh->mNormals[vertex_index].z;
        vertex.normal = v;
        
        if(mesh->mTextureCoords[0])
        {
            vertex.tex_coords.x = mesh->mTextureCoords[0][vertex_index].x;
            vertex.tex_coords.y = mesh->mTextureCoords[0][vertex_index].y;                        
        }
        else
            vertex.tex_coords = create_vec2(0.0f, 0.0f);
        
        vertices[vertex_index] = vertex;        

    }

    // Fill the index array of the mesh
    u32 *indice = result.indices;
    for(u32 indice_index = 0; indice_index < result.index_count; indice_index += 3)
    {
        struct aiFace face = mesh->mFaces[indice_index / 3];
        *indice++ = face.mIndices[0];
        *indice++ = face.mIndices[1];
        *indice++ = face.mIndices[2];                

    }

    // Find a texture and store it
    if(mesh->mMaterialIndex >= 0)
    {
        struct aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];

        u32 diffuse_map_count = aiGetMaterialTextureCount(mat, aiTextureType_DIFFUSE);
        u32 specular_map_count = aiGetMaterialTextureCount(mat, aiTextureType_SPECULAR);
        u32 ambient_map_count = aiGetMaterialTextureCount(mat, aiTextureType_AMBIENT);

        //result.texture_count = diffuse_map_count + specular_map_count + ambient_map_count;

        // @Note: There should be something to allocate, otherwise 'mat' should be null?
        //result.textures = (Texture*)ArenaAlloc16(&memory, resuglt.texture_count * sizeof(Texture));

        // Load its different textures
        // Let's start with diffuse maps and specular maps for now

#if 0
        struct aiString diff_path, amb_path, spec_path;
        u64 path_hash = 0;
        
        if(diffuse_map_count > 0 && aiGetMaterialTexture(mat, aiTextureType_DIFFUSE, 0, &diff_path,
                                 0,0,0,0,0,0))
        {
            Texture tex = {0};
            tex.type = TEX_DIFFUSE;
            char texture_path[256];
            
            strcpy(texture_path, model_folder_path);
            strcat(texture_path, diff_path.data);
            u64 hash = fnv_1a(diff_path.data, diff_path.length);
            
            // Find out if hash already exists
            
            
            tex.id = LoadTexture(texture_path, true);
            
        }

        if(ambient_map_count > 0 && aiGetMaterialTexture(mat, aiTextureType_AMBIENT, 0, &amb_path,
                                 0,0,0,0,0,0))
        {
            
        }

        if(specular_map_count > 0 && aiGetMaterialTexture(mat, aiTextureType_SPECULAR, 0, &spec_path,
                                 0,0,0,0,0,0))
        {
            
        }                
#endif
        
        if(diffuse_map_count > 0)
        {
            struct aiString str;
            aiGetMaterialTexture(mat, aiTextureType_DIFFUSE, 0, &str,
                                 0,0,0,0,0,0);
            
            DiffuseTexture texture;
            char texture_path[256];
            
            strcpy(texture_path, model_folder_path);
            strcat(texture_path, str.data);
            
            texture.id = LoadTexture(texture_path, true);
            //texture.hash = fnv_1a(str.data, str.length); 

            result.material.diffuse_map = texture;            
        }

        if(ambient_map_count > 0)
        {
                
            struct aiString str;
            aiGetMaterialTexture(mat, aiTextureType_AMBIENT, 0, &str,
                                 0,0,0,0,0,0);
            
            AmbientTexture texture;
            char texture_path[256];
            
            strcpy(texture_path, model_folder_path);
            strcat(texture_path, str.data);
            
            texture.id = LoadTexture(texture_path, true);
            result.material.ambient_map = texture;
        }
        

        if(specular_map_count > 0)
        {
                
            struct aiString str;
            aiGetMaterialTexture(mat, aiTextureType_SPECULAR, 0, &str,
                                 0,0,0,0,0,0);

            SpecularTexture texture;
            char texture_path[256];
            
            strcpy(texture_path, model_folder_path);
            strcat(texture_path, str.data);
                        
            texture.id = LoadTexture(texture_path, true);
            result.material.specular_map = texture;            
        }

        struct aiColor4D v;
        if(AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &v))
        {
            result.material.diffuse = create_vec3(v.r, v.g, v.b);
        }

        if(AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &v))
        {
            result.material.specular = create_vec3(v.r, v.g, v.b);
        }

        if(AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &v))
        {
            result.material.ambient = create_vec3(v.r, v.g, v.b);
        }

        float s;
        if(AI_SUCCESS == aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &s))
        {
            result.material.shininess = s;
        }
        else
            result.material.shininess = 32.0f;
        
    }

    result.va = GenVertArr();
    VertexBuffer vbo = GenVertBuf(vertices, result.vertex_count * sizeof(Vertex));
    IndexBuffer ebo = GenIndexBuf(result.indices, result.index_count * sizeof(u32));

    // The layout will have 3 attributes
    VertexLayout va_layout = {0};
    va_layout.attributes = (VertexAttribute*)ArenaAlloc16(scratch, 3 * sizeof(VertexAttribute));
    
    VertLayoutPush(&va_layout, 3, GL_FLOAT, GL_FALSE); // Position
    VertLayoutPush(&va_layout, 3, GL_FLOAT, GL_FALSE); // Normal
    VertLayoutPush(&va_layout, 2, GL_FLOAT, GL_FALSE); // Texture

    BindVertArr(result.va);               
    BindVertBuf(vbo);
    BindIndBuf(ebo);
    VABindLayout(&result.va, va_layout);
    
    UnbindVertArr();
    UnbindVertBuf();
    UnbindIndBuf();    

    return result;
}

// We will process the nodes in a recursive manner
static void ProcessAssimpNode(ArenaMemory *scratch,
                              Model *model,
                              struct aiNode *node,
                              const struct aiScene *scene)
{
    static u32 mesh_index = 0;
    
    struct aiNode *root_node = scene->mRootNode;
    
    for(u32 node_mesh_index = 0; node_mesh_index < node->mNumMeshes; node_mesh_index++)
    {
        struct aiMesh *mesh = scene->mMeshes[node->mMeshes[node_mesh_index]];
        model->meshes[model->mesh_count++] = CreateMeshFromAssimp(scratch, model->model_folder_path, mesh, scene);
    }

    // Process children nodes
    for(u32 child_index = 0; child_index < node->mNumChildren; child_index++)
    {

        ProcessAssimpNode(scratch, model, node->mChildren[child_index], scene);        
    }

}

// Relative to the asset folder
Model LoadModelFromAssimp(ArenaMemory *mesh_memory, ArenaMemory *scratch, u8 *model_folder, u8 *model_name)
{
    Model result = {0};

    char model_path[512];
    strcpy(model_path, "assets\\");
    strcat(model_path, model_folder);        
    strcat(model_path, "\\");

    // Store the relative path to the model folder
    strcpy(result.model_folder_path, model_path);
    
    strcat(model_path, model_name);
    
    const struct aiScene *scene = aiImportFile(model_path, aiProcess_Triangulate);

    assert(scene);
    assert(scene->mFlags | AI_SCENE_FLAGS_INCOMPLETE);

    struct aiNode *root_node = scene->mRootNode;
    
    // Allocate enough meshes for our model
    result.mesh_count = 0;
    result.meshes = (Mesh*) ArenaAlloc16(mesh_memory, scene->mNumMeshes * sizeof(Mesh));
    
    // When loading a model, we will cache up to 64 hashes of the image path
    TextureHashes tex_hashes = {0};
    
    // Begin by processing the root node
    ProcessAssimpNode(scratch, &result, root_node, scene);
           
    return result;
}
