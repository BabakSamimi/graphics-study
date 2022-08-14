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

module char *asset_folder = "assets\\";

// C-strings only
void ConcatStrings(char *dest, char *buf1, char *buf2)
{
    while(*buf1)
    {
        *dest++ = *buf1++;
    }

    while(*buf2)
    {
        *dest++ = *buf2++;
    }

    *dest++ = 0;
    
}

module u32 LoadTexture(u8 *path, s32 flipped)
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

module Mesh CreateMeshFromAssimp(MemoryRegion memory, char *model_folder_path, struct aiMesh *mesh, const struct aiScene *scene)
{
    Mesh result = {0};
    
    result.vertex_count = mesh->mNumVertices;
    result.index_count = mesh->mNumFaces * 3; // @Important: A face could be connected by more than 3 vertices, but if we use aiProcess_Triangulate flag when loading with assimp, then we can always be sure that a face is always a triangle.


    result.vertices = (Vertex*) SliceRegion16(&memory, result.vertex_count * sizeof(Vertex));
    result.indices = (u32*)     SliceRegion16(&memory, result.index_count * sizeof(u32));

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

        
        result.vertices[vertex_index] = vertex;        

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

        u32 texture_index = 0;
        result.texture_count = diffuse_map_count + specular_map_count;

        // @Note: There should be something to allocate, otherwise 'mat' should be null?
        result.textures = (Texture*)SliceRegion16(&memory, result.texture_count * sizeof(Texture));

        // Load its different textures
        // Let's start with diffuse maps and specular maps for now
        for(u32 diffuse_index = 0; diffuse_index < diffuse_map_count; diffuse_index++)
        {
                
            struct aiString str;
            aiGetMaterialTexture(mat, aiTextureType_DIFFUSE, diffuse_index, &str,
                                 0,0,0,0,0,0);
            //Texture texture;
            DiffuseTexture texture;
            char texture_path[256];
            
            strcpy(texture_path, model_folder_path);
            strcat(texture_path, str.data);
            
            texture.id = LoadTexture(texture_path, true);
            //texture.type = DIFFUSE;
            //result.textures[texture_index++] = texture;
            result.material.diffuse_map = texture;
        }

        for(u32 specular_index = 0; specular_index < specular_map_count; specular_index++)
        {
                
            struct aiString str;
            aiGetMaterialTexture(mat, aiTextureType_SPECULAR, specular_index, &str,
                                 0,0,0,0,0,0);
            //Texture texture;
            SpecularTexture texture;
            char texture_path[256];
            
            strcpy(texture_path, model_folder_path);
            strcat(texture_path, str.data);
                        
            texture.id = LoadTexture(texture_path, true);
            //texture.type = SPECULAR;
            //result.textures[texture_index++] = texture;
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
    else
    {
        result.texture_count = 0;
    }

    result.va = GenVertArr();
    VertexBuffer vbo = GenVertBuf(result.vertices, result.vertex_count * sizeof(Vertex));
    IndexBuffer ebo = GenIndexBuf(result.indices, result.index_count * sizeof(u32));

    // The layout will have 3 attributes
    VertexLayout va_layout = {0};
    va_layout.attributes = (VertexAttribute*)SliceRegion16(&memory, 3 * sizeof(VertexAttribute));
    
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
module void ProcessAssimpNode(MemoryRegion memory, Model *model, struct aiNode *node, const struct aiScene *scene)
{
    struct aiNode *root_node = scene->mRootNode;
    
    for(u32 node_mesh_index = 0; node_mesh_index < node->mNumMeshes; node_mesh_index++)
    {
        struct aiMesh *mesh = scene->mMeshes[node->mMeshes[node_mesh_index]];
        model->meshes[model->mesh_count++] = CreateMeshFromAssimp(memory, model->model_folder_path, mesh, scene);
    }

    // Process children nodes
    for(u32 child_index = 0; child_index < node->mNumChildren; child_index++)
    {

        ProcessAssimpNode(memory, model, node->mChildren[child_index], scene);        
    }

}

// Relative to the asset folder
Model LoadModelFromAssimp(MemoryRegion memory, u8 *model_folder, u8 *model_name)
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
    
    result.mesh_count = 0;
    result.meshes = (Mesh*) SliceRegion16(&memory, scene->mNumMeshes * sizeof(Mesh));
    
    // Begin by processing the root node
    ProcessAssimpNode(memory, &result, root_node, scene);
           
    return result;
}
