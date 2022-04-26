/*

  TODO: Handle GLSL shader compilation error, for init and reload

*/

#define SHADER_BUFFER_SIZE (4*1024)
#define SHADER_LOG_SIZE (1*1024)

const char* version_define = "#version 460 core\n";
const char* vertex_define = "#define VERTEX_SHADER\n";
const char* fragment_define = "#define FRAGMENT_SHADER\n";

static u8* paths[][1] = {
    { "default.glsl" },
};

typedef struct {
    u8* (*paths)[1];
    time_t* mod;
    u32* programs;
    u32 programs_count;
    
} shader_bank;

static shader_bank shaders = {0};

s32 FILE_size(FILE* fp)
{
    s32 result;
    
    fseek(fp, 0, SEEK_END);
    result = ftell(fp);
    rewind(fp);
        
    return result;
}

u8* shader_src;

static int init_shader_bank()
{
    shader_src = (u8*)calloc(SHADER_BUFFER_SIZE, sizeof(u8));
    shaders.paths = paths;

    // TODO: Replace malloc/calloc with custom allocator
    shaders.mod = (time_t*)calloc(ArrayCount(paths), sizeof(time_t));    
    shaders.programs_count = ArrayCount(paths);
    shaders.programs = (u32*)calloc(shaders.programs_count, sizeof(u32));
    
    // Populate shader inventory

    for(u8 idx = 0;
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
        s32 file_size;
        s32 bytes_read;
            
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
        bytes_read = fread((void*)shader_src, sizeof(u8), file_size, fp);
        shader_src[file_size + 1] = '\0';        
        fclose(fp);

        const char* const vertex_src[3] = { version_define, vertex_define, shader_src};
        const s32 v_length[3] = {strlen(version_define), strlen(vertex_define), file_size};
        
        const char* const fragment_src[3] = { version_define, fragment_define, shader_src};
        const s32 f_length[3] = {strlen(version_define), strlen(fragment_define), file_size};
                
        char shader_log[SHADER_LOG_SIZE];
                
        // compile vertex shader
        u32 vertex_id = glCreateShader(GL_VERTEX_SHADER);
        
        glShaderSource(vertex_id, 3, vertex_src, v_length);
        glCompileShader(vertex_id);

        s32 vertex_compiled = 0;
        glGetShaderiv(vertex_id, GL_COMPILE_STATUS, &vertex_compiled);        
        if(!vertex_compiled)
        {
            glGetShaderInfoLog(vertex_id, SHADER_LOG_SIZE, 0, shader_log);            
            printf("Vertex shader %s failed! Reason:", shader_path);
            printf("%s\n", shader_log);
        }        
         
        // compile fragment shader
        u32 fragment_id = glCreateShader(GL_FRAGMENT_SHADER);        
        
        glShaderSource(fragment_id, 3, fragment_src, f_length);
        glCompileShader(fragment_id);
        
        s32 fragment_compiled = 0;
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
                
        u32 shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_id);
        glAttachShader(shader_program, fragment_id);
        glLinkProgram(shader_program);
                
        s32 program_linked;
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

static int reload_shader_bank()
{
    
    // Populate shader inventory

    for(u8 idx = 0;
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
        s32 file_size;
        s32 bytes_read;
        u32 shader_changed = 0;
            
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
        bytes_read = fread((void*)shader_src, sizeof(u8), file_size, fp);
        shader_src[file_size + 1] = '\0'; //fread does not append null-terminator
        fclose(fp);

        const char* const vertex_src[3] = { version_define, vertex_define, shader_src};
        const s32 v_length[3] = { strlen(version_define), strlen(vertex_define), file_size };
        
        const char* const fragment_src[3] = { version_define, fragment_define, shader_src};
        const s32 f_length[3] = { strlen(version_define), strlen(fragment_define), file_size };
        char shader_log[1024];
                
        // compile vertex shader
        u32 vertex_id = glCreateShader(GL_VERTEX_SHADER);
        
        glShaderSource(vertex_id, 3, vertex_src, v_length);
        glCompileShader(vertex_id);

        s32 vertex_compiled = 0;
        glGetShaderiv(vertex_id, GL_COMPILE_STATUS, &vertex_compiled);        
        if(!vertex_compiled)
        {
            glGetShaderInfoLog(vertex_id, 1024, 0, shader_log);          
            printf("Vertex shader %s failed! Reason:", shader_path);
            printf("%s\n", shader_log);
            printf("This is the code it tried to compile:\n%s%s%s\n", vertex_src[0], vertex_src[1], vertex_src[2]);
        }        
         
        // compile fragment shader
        u32 fragment_id = glCreateShader(GL_FRAGMENT_SHADER);        
        
        glShaderSource(fragment_id, 3, fragment_src, f_length);
        glCompileShader(fragment_id);
        
        s32 fragment_compiled = 0;
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
                
        u32 shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_id);
        glAttachShader(shader_program, fragment_id);
        glLinkProgram(shader_program);
                
        s32 program_linked;
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
