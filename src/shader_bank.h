#include <time.h> // timespec
#include <stdio.h> // FILE
#include <sys/stat.h> // stat
#include <string.h> // memset, strlen, strcmp
#include <stdlib.h> // calloc

#include "glad/glad.h"

#define SHADER_BUFFER_SIZE (4*1024)
#define SHADER_LOG_SIZE (1*1024)
#define ArrayCount(A) (sizeof((A)) / sizeof((A)[0]))

typedef struct {
    /* Pointer to a 2D array where every row consists of a string */
    unsigned char* (*paths)[2];
    
    /* Timestamp on last modification of the shader file */
    time_t* mod;
    
    /* Programs := internal opengl ids on shader programs */
    unsigned int* programs;
    unsigned int programs_count;

    unsigned int active_program_index;
    
} ShaderBank;

static int FILE_size(FILE* fp);
int init_shader_bank();
int reload_shader_bank();

void use_program_name(unsigned char* program_name);
void use_program(unsigned int program);

void query_program(unsigned int* program, unsigned char* program_name);
void get_active_program(unsigned int* program);
    
void set_float(char* name, float value);
void set_int(char* name, int value);

void set_vec4f(char* name, float* value);
void set_vec3f(char* name, float* value);
void set_vec2f(char* name, float* value);

void set_mat4f(char* name, float* value);
void set_mat3f(char* name, float* value);
