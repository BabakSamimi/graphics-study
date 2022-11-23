#include <sys/stat.h> // stat
#include <string.h> // memset, strlen, strcmp

#include "shader_bank.h"
#include "..\memory.h"

static const u8 *version_define = "#version 460 core\n";
static const u8 *vertex_define = "#define VERTEX_SHADER\n";
static const u8 *fragment_define = "#define FRAGMENT_SHADER\n";

#define SHADER_BUFFER_SIZE (8*1024)
#define SHADER_LOG_SIZE (1*1024)
// #define DEBUG_PRINT_SOURCE

/* Buffer for shader source code, used for compiling shaders */
static u8 *shader_src;
static ArenaMemory mem_reg;

ShaderBank shaders = {0};

static int FILE_size(FILE* fp)
{
    s32 result;
    
    fseek(fp, 0, SEEK_END);
    result = ftell(fp);
    rewind(fp);
        
    return result;
}

void register_shader(char* path, char* name)
{
    shaders.paths[shaders.programs_count][0] = path;
    shaders.paths[shaders.programs_count][1] = name;

    shaders.programs_count++;
}

bool init_shader_bank()
{
    InitArena(&mem_reg, ALLOC_MEM(10*KB(64)), 10*KB(64));

    shader_src = (u8*)       ArenaAlloc16(&mem_reg, SHADER_BUFFER_SIZE * sizeof(unsigned char));
    shaders.mod = (time_t*)             ArenaAlloc16(&mem_reg, shaders.programs_count * sizeof(time_t));
    shaders.programs = (GLuint*)  ArenaAlloc16(&mem_reg, shaders.programs_count * sizeof(unsigned int));
    shaders.active_program_index = 0;
    
    // Populate shader bank
    return reload_shader_bank();
	 
}

bool reload_shader_bank()
{
    bool status = true;

    for(u8 idx = 0;
        idx < shaders.programs_count;
        idx++)
    {
        
        u8 *shader_path = shaders.paths[idx][0];     
        FILE *fp = fopen(shader_path, "rb");

        if(!fp)
        {
            printf("Could not open shader file: %s\n", shader_path);
            status = false;
            fclose(fp);            
            continue;
        }

        struct stat fstat;
        s32 file_size;
        size_t bytes_read;
        u32 shader_changed = 0;
            
        // save last modification
        stat(shader_path, &fstat);
        if(fstat.st_mtime > shaders.mod[idx])
        {
            shaders.mod[idx] = fstat.st_mtime;
            shader_changed = true;
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
            status = false;
            continue;
        }
        else if (file_size > SHADER_BUFFER_SIZE)
        {
            printf("File size too big! File was %d bytes bigger than limit.\n", file_size - SHADER_BUFFER_SIZE);
            fclose(fp);
            status = false;
            continue;
        }

        // read shader file
        bytes_read = fread((void*)shader_src, sizeof(unsigned char), file_size, fp);
        fclose(fp);
        
        shader_src[file_size] = '\0'; //fread does not append null-terminator

        // vertex_src and fragment_src copies shader_src, which is unneccesary copying
        // TODO: Carve out the source code for the different shaders out of shader_src
        // This can yield to more readable error printing when printing source code to
        // the console.
        const u8 *const vertex_src[3] = { version_define, vertex_define, shader_src };
        const int v_length[3] = { strlen(version_define), strlen(vertex_define), file_size };
        
        const u8 *const fragment_src[3] = { version_define, fragment_define, shader_src };
        const int f_length[3] = { strlen(version_define), strlen(fragment_define), file_size };
        
        u8 shader_log[SHADER_LOG_SIZE];
                
        /* Compile vertex shader */
        GLuint vertex_id = glCreateShader(GL_VERTEX_SHADER);
        
        glShaderSource(vertex_id, 3, vertex_src, v_length);
        glCompileShader(vertex_id);

        s32 vertex_compiled = 0;
        glGetShaderiv(vertex_id, GL_COMPILE_STATUS, &vertex_compiled);        
        if(!vertex_compiled)
        {
            glGetShaderInfoLog(vertex_id, SHADER_LOG_SIZE, 0, shader_log);          
            printf("Vertex shader %s failed! Reason: %s\n", shader_path, shader_log);
            
#if DEBUG_PRINT_SOURCE
            printf("This is the code it tried to compile:\n%s\n", vertex_src[2]);
#endif
            
        }        
         
        /* Compile fragment shader */
        GLuint fragment_id = glCreateShader(GL_FRAGMENT_SHADER);        
        
        glShaderSource(fragment_id, 3, fragment_src, f_length);
        glCompileShader(fragment_id);
        
        s32 fragment_compiled = 0;
        glGetShaderiv(fragment_id, GL_COMPILE_STATUS, &fragment_compiled);
        if(!fragment_compiled)
        {
            glGetShaderInfoLog(fragment_id, SHADER_LOG_SIZE, 0, shader_log);            
            printf("Fragment shader %s failed! Reason: %s\n", shader_path, shader_log);
            
#if DEBUG_PRINT_SOURCE            
            printf("This is the code it tried to compile:\n%s\n", fragment_src[2]);
#endif
            
        }

        /* If one of them failed, don't even try to link them together */
        if(!vertex_compiled || !fragment_compiled)
        {            
            glDeleteShader(vertex_id);
            glDeleteShader(fragment_id);
            status = false;
            continue;
        }
                
        GLuint shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_id);
        glAttachShader(shader_program, fragment_id);
        glLinkProgram(shader_program);
                
        s32 program_linked;
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
	
    return status;    
}

void use_program_name(u8 *program_name)
{
    /* Naive */
    for(unsigned index = 0; index < shaders.programs_count; index++)
    {
        if(!strcmp(shaders.paths[index][1], program_name))
        {
            shaders.active_program_index = index;
            glUseProgram(shaders.programs[index]);
            break;
        }
    }

}

void use_program(GLuint program)
{

    for(s32 index = 0; index < shaders.programs_count; index++)
    {
        if(program == shaders.programs[index])
        {
            shaders.active_program_index = index;
            glUseProgram(program);
            break;
        }

    }
    
}

void query_program(GLuint *program, u8 *program_name)
{

    /* Naive */
    for(unsigned index = 0; index < shaders.programs_count; index++)
    {
        if(!strcmp(shaders.paths[index][1], program_name))
        {
            *program = shaders.programs[index];
            break;
        }
                
    }
    
}

void get_active_program(GLuint *program)
{
    *program = shaders.programs[shaders.active_program_index];
}

void set_float(u8 *name, float val)
{
    glUniform1f(glGetUniformLocation(shaders.programs[shaders.active_program_index], name), val);
}

void set_int(u8 *name, int val)
{
    glUniform1i(glGetUniformLocation(shaders.programs[shaders.active_program_index], name), val);    
}

void set_vec4f(u8 *name, vec4 v)
{
    glUniform4f(glGetUniformLocation(shaders.programs[shaders.active_program_index], name), v.x, v.y, v.z, v.w);    
}

void set_vec3f(u8 *name, vec3 v)
{
    glUniform3f(glGetUniformLocation(shaders.programs[shaders.active_program_index], name), v.x, v.y, v.z);        
}

void set_vec2f(u8 *name, vec2 v)
{
    glUniform2f(glGetUniformLocation(shaders.programs[shaders.active_program_index], name), v.x, v.y);        
}

void set_mat4f(u8 *name, float* val)
{
    glUniformMatrix4fv(glGetUniformLocation(shaders.programs[shaders.active_program_index], name), 1, GL_FALSE, val);
}

void set_mat3f(u8 *name, float* val)
{
    glUniformMatrix3fv(glGetUniformLocation(shaders.programs[shaders.active_program_index], name), 1, GL_FALSE, val);
}
