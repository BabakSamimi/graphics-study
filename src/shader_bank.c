/*

  TODO: Handle GLSL shader compilation error, for init and reload

*/

#define SHADER_BUFFER_SIZE (4*1024)

static u8* paths[][2] = {
    { "vertex.glsl", "fragment.glsl" },
    { "vertex.glsl", "fragment.glsl" },
    { "vertex.glsl", "fragment.glsl" },      
};

/*

  Shader bank  memory layout:
  u8*
  time_t: [v1_mod, v2_mod, f1_mod] -- this is the timestamp for last modification
  u32:    gl_handles -- indexed via vertex/fragment_count
  
*/

typedef struct {

    u8* (*paths)[2];    
    time_t* mod;
    u32* programs;
    u32* gl_handles;
    
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

static int init_shader_bank()
{

    shaders.paths = paths;

    // TODO: Replace malloc/calloc with custom allocator
    shaders.mod = (time_t*)calloc(ArrayCount(paths)*2, sizeof(time_t));
    shaders.gl_handles = (u32*)calloc(ArrayCount(paths)*2, sizeof(u32));
    shaders.programs = (u32*)calloc(ArrayCount(paths), sizeof(u32));
    
    printf("Rows: %zd\n", ArrayCount(paths));
    printf("Columns: %zd\n", ArrayCount(paths[0]));
    
    u8* shader_src = (u8*)calloc(SHADER_BUFFER_SIZE, sizeof(u8));
    
    // Populate shader inventory    

    for(u8 programme_idx = 0;
        programme_idx == ArrayCount(shaders.paths);
        programme_idx += 2)
    {
        
        char* vertex_path = shaders.paths[programme_idx][0];
        char* fragment_path = shaders.paths[programme_idx][1];
        FILE* fp_v = fopen(vertex_path, "rb");
        FILE* fp_f = fopen(fragment_path, "rb");

        if(!fp_v)
        {
            printf("Could not open %s\n", vertex_path);
            fclose(fp_v);
            fclose(fp_f);
            continue;
        }

        if(!fp_f)
        {
            printf("Could not open %s\n", fragment_path);
            fclose(fp_v);
            fclose(fp_f);            
            continue;
        }

        struct stat fstat;
        s32 file_size;
        s32 bytes_read;
            
        // save last modification
        stat(vertex_path, &fstat);
        shaders.mod[programme_idx] = fstat.st_mtime;
        stat(fragment_path, &fstat);
        shaders.mod[programme_idx+1] = fstat.st_mtime;

        // read the shaders and compile
        file_size = FILE_size(fp_v);
        if (file_size == -1L)
        {
            printf("Could not evaluate file size for the vertex shader: %s\n", vertex_path);
            fclose(fp_v);
            fclose(fp_f);
            continue;
        }

        // read vertex shader
        bytes_read = fread((void*)shader_src, sizeof(u8), file_size, fp_v);        
        
        // compile vertex shader
        u32* vertex_id = &shaders.gl_handles[programme_idx];
        *vertex_id = glCreateShader(GL_VERTEX_SHADER);
        
        glShaderSource(*vertex_id, 1, &shader_src, &file_size);
        glCompileShader(*vertex_id);
        
        file_size = FILE_size(fp_f);
        if (file_size == -1L)
        {
            printf("Could not evaluate file size for the fragment shader: %s\n", fragment_path);
            fclose(fp_v);
            fclose(fp_f);            
            continue;
        }

        // read fragment shader
        bytes_read = fread((void*)shader_src, sizeof(u8), file_size, fp_f);
        
        // compile fragment shader
        u32* fragment_id = &shaders.gl_handles[programme_idx+1];
        *fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
        
        glShaderSource(*fragment_id, 1, &shader_src, &file_size);
        glCompileShader(*fragment_id);

        u32 shader_program = glCreateProgram();
        glAttachShader(shader_program, *vertex_id);
        glAttachShader(shader_program, *fragment_id);
        glLinkProgram(shader_program);
        
        shaders.programs[(u32)(programme_idx/2)] = shader_program;

        glDeleteShader(*vertex_id);
        glDeleteShader(*fragment_id);        
        
        fclose(fp_v);
        fclose(fp_f);       
    }

    free(shader_src);
	
    return 0;
}

// only reloads shaders if they were changed
static int reload_shader_bank()
{    
    u8* shader_src = (u8*)calloc(SHADER_BUFFER_SIZE, sizeof(u8));
    
    // Populate shader inventory    

    for(u8 programme_idx = 0;
        programme_idx == ArrayCount(shaders.paths);
        programme_idx += 2)
    {
        
        char* vertex_path = shaders.paths[programme_idx][0];
        char* fragment_path = shaders.paths[programme_idx][1];
        FILE* fp_v = fopen(vertex_path, "rb");
        FILE* fp_f = fopen(fragment_path, "rb");

        if(!fp_v)
        {
            printf("Could not open %s\n", vertex_path);
            fclose(fp_v);
            fclose(fp_f);
            continue;
        }

        if(!fp_f)
        {
            printf("Could not open %s\n", fragment_path);
            fclose(fp_v);
            fclose(fp_f);            
            continue;
        }

        struct stat fstat;
        s32 file_size;
        s32 bytes_read;
        u32 shader_changed = 0;
            
        // save last modification
        stat(vertex_path, &fstat);
        if(fstat.st_mtime > shaders.mod[programme_idx])
        {
            shaders.mod[programme_idx] = fstat.st_mtime;
            shader_changed = 1;
        }
        
        stat(fragment_path, &fstat);
        if(fstat.st_mtime > shaders.mod[programme_idx+1])
        {
            shaders.mod[programme_idx] = fstat.st_mtime;
            shader_changed = shader_changed ? 3 : 2;
        }

        if(!shader_changed) {
            
            fclose(fp_v);
            fclose(fp_f);
            continue;
        }

        // read the shaders and compile
        file_size = FILE_size(fp_v);
        if (file_size == -1L)
        {
            printf("Could not evaluate file size for the vertex shader: %s\n", vertex_path);
            fclose(fp_v);
            fclose(fp_f);
            continue;
        }

        // TODO: If compilation failed for either of the shaders, continue to next loop
        
        // read vertex shader
        bytes_read = fread((void*)shader_src, sizeof(u8), file_size, fp_v);        
        
        // compile vertex shader
        u32* vertex_id = &shaders.gl_handles[programme_idx];
        *vertex_id = glCreateShader(GL_VERTEX_SHADER);
        
        glShaderSource(*vertex_id, 1, &shader_src, &file_size);
        glCompileShader(*vertex_id);
        
        file_size = FILE_size(fp_f);
        if (file_size == -1L)
        {
            printf("Could not evaluate file size for the fragment shader: %s\n", fragment_path);
            fclose(fp_v);
            fclose(fp_f);            
            continue;
        }

        // read fragment shader
        bytes_read = fread((void*)shader_src, sizeof(u8), file_size, fp_f);
        
        // compile fragment shader
        u32* fragment_id = &shaders.gl_handles[programme_idx+1];
        *fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
        
        glShaderSource(*fragment_id, 1, &shader_src, &file_size);
        glCompileShader(*fragment_id);

        u32 shader_program = glCreateProgram();
        glAttachShader(shader_program, *vertex_id);
        glAttachShader(shader_program, *fragment_id);
        glLinkProgram(shader_program);

        // TODO: if link failed, don't replace the current program
        shaders.programs[(u32)(programme_idx/2)] = shader_program;

        glDeleteShader(*vertex_id);
        glDeleteShader(*fragment_id);        
        
        fclose(fp_v);
        fclose(fp_f);

        if(shader_changed == 1)
            printf("Successfully reloaded %s\n", vertex_path);
        else if(shader_changed == 2)
            printf("Successfully reloaded %s\n", fragment_path);
        else if(shader_changed == 3)
            printf("Successfully reloaded %s and %s\n", vertex_path, fragment_path);        
    }

    free(shader_src);

	
    return 0;    
}
