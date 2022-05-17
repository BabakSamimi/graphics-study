/*

  TODO: Handle GLSL shader compilation error, for init and reload

*/

#include "shader_bank.h"

static const char* version_define = "#version 460 core\n";
static const char* vertex_define = "#define VERTEX_SHADER\n";
static const char* fragment_define = "#define FRAGMENT_SHADER\n";

static unsigned char* paths[][1] = {
    { "src\\default.glsl" },
};
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
    shader_src = (unsigned char*)calloc(SHADER_BUFFER_SIZE, sizeof(unsigned char));
    shaders.paths = paths;

    // TODO: Replace malloc/calloc with custom allocator
    shaders.mod = (time_t*)calloc(ArrayCount(paths), sizeof(time_t));    
    shaders.programs_count = ArrayCount(paths);
    shaders.programs = (unsigned int*)calloc(shaders.programs_count, sizeof(unsigned int));
    
    // Populate shader inventory

    for(unsigned char idx = 0;
        idx < shaders.programs_count;
        idx++)
    {

        char* shader_path = shaders.paths[idx][0];     
        FILE* fp = fopen(shader_path, "rb");

        printf("Processing shader: %s\n", shader_path);
        
        if(!fp)
        {
            printf("Could not open shader file: %s\n", shader_path);
            fclose(fp);            
            continue;
        }

        struct stat fstat;
        int file_size;
        int bytes_read;
            
        // save last modification
        stat(shader_path, &fstat);
        shaders.mod[idx] = fstat.st_mtime;

        file_size = FILE_size(fp);
        if (file_size == -1L)
        {
            printf("Could not evaluate file size for the shader: %s\n", shader_path);
            fclose(fp);
            continue;
        }

        // read shader file
        bytes_read = fread((void*)shader_src, sizeof(unsigned char), file_size, fp);
        shader_src[file_size + 1] = '\0';        
        fclose(fp);

        const char* const vertex_src[3] = { version_define, vertex_define, shader_src};
        const int v_length[3] = {strlen(version_define), strlen(vertex_define), file_size};
        
        const char* const fragment_src[3] = { version_define, fragment_define, shader_src};
        const int f_length[3] = {strlen(version_define), strlen(fragment_define), file_size};
                
        char shader_log[SHADER_LOG_SIZE];
                
        // compile vertex shader
        unsigned int vertex_id = glCreateShader(GL_VERTEX_SHADER);
        
        glShaderSource(vertex_id, 3, vertex_src, v_length);
        glCompileShader(vertex_id);

        int vertex_compiled = 0;
        glGetShaderiv(vertex_id, GL_COMPILE_STATUS, &vertex_compiled);        
        if(!vertex_compiled)
        {
            glGetShaderInfoLog(vertex_id, SHADER_LOG_SIZE, 0, shader_log);            
            printf("Vertex shader %s failed! Reason:", shader_path);
            printf("%s\n", shader_log);
        }        
         
        // compile fragment shader
        unsigned int fragment_id = glCreateShader(GL_FRAGMENT_SHADER);        
        
        glShaderSource(fragment_id, 3, fragment_src, f_length);
        glCompileShader(fragment_id);
        
        int fragment_compiled = 0;
        glGetShaderiv(fragment_id, GL_COMPILE_STATUS, &fragment_compiled);
        if(!fragment_compiled)
        {
            glGetShaderInfoLog(fragment_id, SHADER_LOG_SIZE, 0, shader_log);
            printf("Fragment shader %s failed! Reason:", shader_path);
            printf("%s\n", shader_log);
        }

        // If one of them failed, don't even try to link them together
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
        if(!program_linked)
        {
            glGetProgramInfoLog(shader_program, SHADER_LOG_SIZE, 0, shader_log);            
            printf("Linking failed for %s! Reason:", shader_path);
            printf("%s\n", shader_log);
        }
        else
        {
            shaders.programs[idx] = shader_program;
            printf("Compiled and linked shader: %s\n", shader_path);
        }
        
        glDeleteShader(vertex_id);
        glDeleteShader(fragment_id);

    }

    //free(shader_src);
	
    return 0;    
}

int reload_shader_bank()
{
    
    // Populate shader inventory

    for(unsigned char idx = 0;
        idx < shaders.programs_count;
        idx++)
    {
        
        char* shader_path = shaders.paths[idx][0];     
        FILE* fp = fopen(shader_path, "rb");

        if(!fp)
        {
            printf("Could not open shader file: %s\n", shader_path);
            fclose(fp);            
            continue;
        }

        struct stat fstat;
        int file_size;
        int bytes_read;
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

        printf("Reloading shader %s\n", shader_path);

        // read the shaders and compile
        file_size = FILE_size(fp);
        if (file_size == -1L)
        {
            printf("Could not evaluate file size for the shader: %s\n", shader_path);
            fclose(fp);
            continue;
        }

        // read shader file
        bytes_read = fread((void*)shader_src, sizeof(unsigned char), file_size, fp);
        shader_src[file_size + 1] = '\0'; //fread does not append null-terminator
        fclose(fp);

        const char* const vertex_src[3] = { version_define, vertex_define, shader_src};
        const int v_length[3] = { strlen(version_define), strlen(vertex_define), file_size };
        
        const char* const fragment_src[3] = { version_define, fragment_define, shader_src};
        const int f_length[3] = { strlen(version_define), strlen(fragment_define), file_size };
        char shader_log[1024];
                
        // compile vertex shader
        unsigned int vertex_id = glCreateShader(GL_VERTEX_SHADER);
        
        glShaderSource(vertex_id, 3, vertex_src, v_length);
        glCompileShader(vertex_id);

        int vertex_compiled = 0;
        glGetShaderiv(vertex_id, GL_COMPILE_STATUS, &vertex_compiled);        
        if(!vertex_compiled)
        {
            glGetShaderInfoLog(vertex_id, 1024, 0, shader_log);          
            printf("Vertex shader %s failed! Reason:", shader_path);
            printf("%s\n", shader_log);
            printf("This is the code it tried to compile:\n%s%s%s\n", vertex_src[0], vertex_src[1], vertex_src[2]);
        }        
         
        // compile fragment shader
        unsigned int fragment_id = glCreateShader(GL_FRAGMENT_SHADER);        
        
        glShaderSource(fragment_id, 3, fragment_src, f_length);
        glCompileShader(fragment_id);
        
        int fragment_compiled = 0;
        glGetShaderiv(fragment_id, GL_COMPILE_STATUS, &fragment_compiled);
        if(!fragment_compiled)
        {
            glGetShaderInfoLog(fragment_id, 1024, 0, shader_log);            
            printf("Fragment shader %s failed! Reason: ", shader_path);
            printf("%s\n", shader_log);
            printf("This is the code it tried to compile:\n %s %s %s\n", fragment_src[0], fragment_src[1], fragment_src[2]);
        }

        // If one of them failed, don't even try to link them together
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
        if(!program_linked)
        {
            glGetProgramInfoLog(shader_program, 1024, 0, shader_log);            
            printf("Linking failed for %s! Reason:", shader_path);
            printf("%s\n", shader_log);
        }
        else
        {
            shaders.programs[idx] = shader_program;
            printf("Compiled and linked shader: %s\n", shader_path);
        }

        glDeleteShader(vertex_id);
        glDeleteShader(fragment_id);
        printf("\n");
     
    }

    //free(shader_src);
	
    return 0;    
}
