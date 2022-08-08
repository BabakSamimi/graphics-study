#ifndef SHADER_BANK_H
#define SHADER_BANK_H
#include <time.h> // timespec
#include <stdio.h> // FILE
#include <stdbool.h>

#include "glad/glad.h"

#define ArrayCount(A) (sizeof((A)) / sizeof((A)[0]))
#define MAX_SHADER_PROGRAMS 64

typedef struct {
    
    /* Pointer to a 2D array where every row consists of a shader program (a path and its name) */
    unsigned char* paths[MAX_SHADER_PROGRAMS][2];
    
    /* Timestamp on last modification of the shader file */
    time_t* mod;
    
    /* Programs := array of internal opengl ids on shader programs */
    unsigned int* programs;
    unsigned int programs_count;

    unsigned int active_program_index;
    
} ShaderBank;

static int FILE_size(FILE* fp);
void register_shader(char* path, char* name);
bool init_shader_bank();
bool reload_shader_bank();

void use_program_name(char* program_name);
void use_program(unsigned int program);

void query_program(unsigned int* program, char* program_name);
void get_active_program(unsigned int* program);
    
void set_float(char* name, float value);
void set_int(char* name, int value);

void set_vec4f(char* name, float a, float b, float c, float d);
void set_vec3f(char* name, float a, float b, float c);
void set_vec2f(char* name, float a, float b);

void set_mat4f(char* name, float* value);
void set_mat3f(char* name, float* value);

#endif
