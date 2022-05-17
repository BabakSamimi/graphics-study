#include <time.h> // timespec
#include <stdio.h> // FILE
#include <sys/stat.h> // stat
#include <string.h> // memset, strlen
#include <stdlib.h> // calloc

#include "glad/glad.h"

#define SHADER_BUFFER_SIZE (4*1024)
#define SHADER_LOG_SIZE (1*1024)
#define ArrayCount(A) (sizeof((A)) / sizeof((A)[0]))

typedef struct {
    unsigned char* (*paths)[1];
    time_t* mod;
    unsigned int* programs;
    unsigned int programs_count;
    
} ShaderBank;

static int FILE_size(FILE* fp);
int init_shader_bank();
int reload_shader_bank();
