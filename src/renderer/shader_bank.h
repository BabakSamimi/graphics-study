#ifndef SHADER_BANK_H
#define SHADER_BANK_H
#include <time.h> // timespec
#include <stdio.h> // FILE
#include <stdbool.h>

#include "glad/glad.h"
#include "..\gfx_math.h"
#include "..\defines.h"

#define MAX_SHADER_PROGRAMS 64

typedef struct {
    
    /* Pointer to a 2D array where every row consists of a shader program (a path and its name) */
    u8* paths[MAX_SHADER_PROGRAMS][2];
    
    /* Timestamp on last modification of the shader file */
    time_t *mod;
    
    /* Programs := array of internal opengl ids on shader programs */
    GLuint *programs;
    u32 programs_count;

    u32 active_program_index;
    
} ShaderBank;

module int FILE_size(FILE* fp);
void register_shader(char* path, char* name);
bool init_shader_bank();
bool reload_shader_bank();

void use_program_name(u8 *program_name);
void use_program(GLuint program);

void query_program(GLuint *program, u8 *program_name);
void get_active_program(GLuint *program);
    
void set_float(u8 *name, f32 value);
void set_int(u8 *name, int value);

void set_vec4f(u8 *name, vec4 v);
void set_vec3f(u8 *name, vec3 v);
void set_vec2f(u8 *name, vec2 v);

void set_mat4f(u8 *name, float* value);
void set_mat3f(u8 *name, float* value);

#endif
