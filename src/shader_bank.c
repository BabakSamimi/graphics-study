/*

  TODO: Handle GLSL shader compilation error, for init and reload

*/

#include "shader_bank.h"

static const char* version_define = "#version 460 core\n";
static const char* vertex_define = "#define VERTEX_SHADER\n";
static const char* fragment_define = "#define FRAGMENT_SHADER\n";

static unsigned char* paths[][2] = {
    { "src\\cube.glsl", "cube" },
    { "src\\light.glsl", "light" },    
  
};

/* List of faulty programs */
// NOTE: Only allocated for 64 programs 
#define MAX_FAULTY_PROGRAMS 64
static unsigned char* faulty_program_names[MAX_FAULTY_PROGRAMS] = {0};
static unsigned int faulty_program_handles[MAX_FAULTY_PROGRAMS] = {0};

#define SHADER_BUFFER_SIZE (4*1024)
#define SHADER_LOG_SIZE (1*1024)

/* Buffer for shader source code, used for compiling shaders */
static unsigned char* shader_src;

ShaderBank shaders = {0};

static int FILE_size(FILE* fp)
{
    int result;
    
    fseek(fp, 0, SEEK_END);
    result = ftell(fp);
    rewind(fp);
        
    return result;
}

int init_shader_bank()
{
    shaders.active_program_index = 0;
    shader_src = (unsigned char*)calloc(SHADER_BUFFER_SIZE, sizeof(unsigned char));
    shaders.paths = paths;

    /* TODO: Replace malloc/calloc with custom allocator */
    shaders.mod = (time_t*)calloc(ArrayCount(paths), sizeof(time_t)); // calloc inits everything to zero
    shaders.programs_count = ArrayCount(paths);
    shaders.programs = (unsigned int*)calloc(shaders.programs_count, sizeof(unsigned int));
    
    
    // Populate shader bank
    reload_shader_bank();
	
    return 0;    
}

int reload_shader_bank()
{   

    for(unsigned char idx = 0;
        idx < shaders.programs_count;
        idx++)
    {
        
        unsigned char* shader_path = shaders.paths[idx][0];     
        FILE* fp = fopen(shader_path, "rb");

        if(!fp)
        {
            printf("Could not open shader file: %s\n", shader_path);
            fclose(fp);            
            continue;
        }

        struct stat fstat;
        int file_size;
        size_t bytes_read;
        unsigned int shader_changed = 0;
            
        // save last modification
        stat(shader_path, &fstat);
        if(fstat.st_mtime > shaders.mod[idx])
        {
            shaders.mod[idx] = fstat.st_mtime;
            shader_changed = 1;
        }

        if(!shader_changed)
        {                     
            fclose(fp);
            continue;
        }

        printf("Loading shader %s\n", shader_path);

        file_size = FILE_size(fp);
        if (file_size == -1L)
        {
            printf("Could not evaluate file size for the shader: %s\n", shader_path);
            fclose(fp);
            continue;
        }

        // read shader file
        bytes_read = fread((void*)shader_src, sizeof(unsigned char), file_size, fp);
        fclose(fp);
        
        shader_src[file_size] = '\0'; //fread does not append null-terminator

        /* vertex_src and fragment_src copies shader_src, which is unneccesary copying */
        const unsigned char* const vertex_src[3] = { version_define, vertex_define, shader_src };
        const int v_length[3] = { strlen(version_define), strlen(vertex_define), file_size };
        
        const unsigned char* const fragment_src[3] = { version_define, fragment_define, shader_src };
        const int f_length[3] = { strlen(version_define), strlen(fragment_define), file_size };
        
        char shader_log[SHADER_LOG_SIZE];
                
        /* Compile vertex shader */
        unsigned int vertex_id = glCreateShader(GL_VERTEX_SHADER);
        
        glShaderSource(vertex_id, 3, vertex_src, v_length);
        glCompileShader(vertex_id);

        int vertex_compiled = 0;
        glGetShaderiv(vertex_id, GL_COMPILE_STATUS, &vertex_compiled);        
        if(!vertex_compiled)
        {
            glGetShaderInfoLog(vertex_id, SHADER_LOG_SIZE, 0, shader_log);          
            printf("Vertex shader %s failed! Reason: %s\n", shader_path, shader_log);
            printf("This is the code it tried to compile:\n%s\n", vertex_src[2]);
        }        
         
        /* Compile fragment shader */
        unsigned int fragment_id = glCreateShader(GL_FRAGMENT_SHADER);        
        
        glShaderSource(fragment_id, 3, fragment_src, f_length);
        glCompileShader(fragment_id);
        
        int fragment_compiled = 0;
        glGetShaderiv(fragment_id, GL_COMPILE_STATUS, &fragment_compiled);
        if(!fragment_compiled)
        {
            glGetShaderInfoLog(fragment_id, SHADER_LOG_SIZE, 0, shader_log);            
            printf("Fragment shader %s failed! Reason: %s\n", shader_path, shader_log);
            printf("This is the code it tried to compile:\n%s\n", fragment_src[2]);
        }

        /* If one of them failed, don't even try to link them together */
        if(!vertex_compiled || !fragment_compiled)
        {            
            glDeleteShader(vertex_id);
            glDeleteShader(fragment_id);                    
            continue;
        }
                
        unsigned int shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_id);
        glAttachShader(shader_program, fragment_id);
        glLinkProgram(shader_program);
                
        int program_linked;
        glGetProgramiv(shader_program, GL_LINK_STATUS, &program_linked);
        
        if(program_linked)
        {
            shaders.programs[idx] = shader_program;
            printf("Compiled and linked shader program: %s\n\n", shaders.paths[idx][1]);
        }
        else
        {
            glGetProgramInfoLog(shader_program, SHADER_LOG_SIZE, 0, shader_log);            
            printf("Linking failed for %s! Reason: %s\n\n", shader_path, shader_log);
        }

        glDeleteShader(vertex_id);
        glDeleteShader(fragment_id);
     
    }
	
    return 0;    
}

void use_program_name(unsigned char* program_name)
{

    int found = 0;
    
    /* Naive */
    for(unsigned index = 0; index < shaders.programs_count; index++)
    {
        if(!strcmp(shaders.paths[index][1], program_name))
        {
            found = 1;
            shaders.active_program_index = index;
            glUseProgram(shaders.programs[index]);
            break;
        }
    }

    if(!found)
    {
        found = 0; // reuse
        unsigned int f_index = 0;
        char* faulty_program = faulty_program_names[f_index];
            
        while(faulty_program)
        {
            if(faulty_program == program_name)
            {
                found = 1;
                break;
            }
                
            faulty_program = faulty_program_names[++f_index];
        }

        if(!found)
        {
            faulty_program_names[f_index] = program_name;
            printf("Could not use shader program '%s'. Reason: Does not exist.\n", program_name);                             
        }        
    }
}

void use_program(unsigned int program)
{
    int found = 0;
    for(unsigned index = 0; index < shaders.programs_count; index++)
    {
        if(program == shaders.programs[index])
        {
            found = 1;
            shaders.active_program_index = index;
            glUseProgram(program);
            break;
        }

    }

    if(!found)
    {
        found = 0; // reuse
        unsigned int f_index = 0;
        int faulty_program = faulty_program_handles[f_index];
            
        while(faulty_program)
        {
            if(faulty_program == program)
            {
                found = 1;
                break;
            }
                
            faulty_program = faulty_program_handles[++f_index];
        }

        if(!found)
        {
            faulty_program_handles[f_index] = program;
            printf("Could not use shader program with id '%d'. Reason: Does not exist.\n", program);
        }

    }
    
}

void query_program(unsigned int* program, unsigned char* program_name)
{
    int found = 0;
    /* Naive */
    for(unsigned index = 0; index < shaders.programs_count; index++)
    {
        if(!strcmp(shaders.paths[index][1], program_name))
        {
            found = 1;
            *program = shaders.programs[index];
            break;
        }
                
    }
    
    if(!found)
    {
        found = 0; // reuse
        unsigned int f_index = 0;
        char* faulty_program = faulty_program_names[f_index];
            
        while(faulty_program)
        {
            if(faulty_program == program_name)
            {
                found = 1;
                break;
            }
                
            faulty_program = faulty_program_names[++f_index];
        }

        if(!found)
        {
            faulty_program_names[f_index] = program_name;
            printf("Could not query shader program '%s'. Reason: Does not exist.\n", program_name);                             
        }        
    }
    
}

void get_active_program(unsigned int* program)
{
    *program = shaders.programs[shaders.active_program_index];
}

void set_float(char* name, float val)
{
    glUniform1f(glGetUniformLocation(shaders.programs[shaders.active_program_index], name), val);
}

void set_int(char* name, int val)
{
    glUniform1i(glGetUniformLocation(shaders.programs[shaders.active_program_index], name), val);    
}

void set_vec4f(char* name, float* val)
{
    glUniform4f(glGetUniformLocation(shaders.programs[shaders.active_program_index], name), val[0], val[1], val[2], val[3]);    
}

void set_vec3f(char* name, float* val)
{
    glUniform3f(glGetUniformLocation(shaders.programs[shaders.active_program_index], name), val[0], val[1], val[2]);        
}

void set_vec2f(char* name, float* val)
{
    glUniform2f(glGetUniformLocation(shaders.programs[shaders.active_program_index], name), val[0], val[1]);        
}

void set_mat4f(char* name, float* val)
{
    glUniformMatrix4fv(glGetUniformLocation(shaders.programs[shaders.active_program_index], name), 1, GL_FALSE, val);
}

void set_mat3f(char* name, float* val)
{
    glUniformMatrix3fv(glGetUniformLocation(shaders.programs[shaders.active_program_index], name), 1, GL_FALSE, val);
}
