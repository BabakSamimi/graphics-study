#include "renderer.h"
#include "camera.h"
#include "vertex_array.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "shader_bank.h"
#include "model.h"

#include "cube.h"

#include "..\win64_main.h"
#include "..\defines.h"
#include "..\gfx_math.h"

#include "GLFW/glfw3.h"

module vec3 light_pos;
module vec3 light_color;
module vec3 light_dir;
module Model test_model;

module ArenaMemory region1;
module ArenaMemory scratch_memory;

Camera global_cam;
extern AppState app_state;
extern ShaderBank shaders;

module void APIENTRY GLDebugOutput(GLenum source, GLenum type, GLuint id,
                              GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{

    char *source_str, *type_str, *severity_str;
    if(severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

    switch(source)
    {
        case GL_DEBUG_SOURCE_API: source_str = "OpenGL API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: source_str = "Window system"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: source_str = "Shader compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: source_str = "Third party"; break;
        case GL_DEBUG_SOURCE_APPLICATION: source_str = "Application"; break;
        case GL_DEBUG_SOURCE_OTHER: source_str = "Other"; break;
        default: source_str = "Unknown source";
    }

    switch(type)
    {
        case GL_DEBUG_TYPE_ERROR: type_str = "OpenGL API"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_str = "Deprecated behavior"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: type_str = "Undefined behavior"; break;
        case GL_DEBUG_TYPE_PORTABILITY: type_str = "Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE: type_str = "Performance"; break;
        case GL_DEBUG_TYPE_MARKER: type_str = "Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP: type_str = "Push group"; break;
        case GL_DEBUG_TYPE_POP_GROUP: type_str = "Pop group"; break;
        case GL_DEBUG_TYPE_OTHER:
        default: type_str = "Unknown type";
    }

    switch(severity)
    {
        case GL_DEBUG_SEVERITY_HIGH: severity_str = "High severity"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: severity_str = "Medium severity"; break;
        case GL_DEBUG_SEVERITY_LOW: severity_str = "Low severity"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: severity_str = "Notification"; break;
        default: severity_str = "Unknown severity";
    }
    
    printf("[OpenGL] (%d) [Source: %s] [Type: %s] [Severity: %s]:\n %s\n\n", id, source_str, type_str, severity_str, message);
    
}

// unit square, first quadrant of NDC
module f32 quad_vert[] = {
    0.0f, 1.0f, 0.0f, // (0, 1)
    1.0f, 0.0f, 0.0f, // (1, 0)

    0.0f, 0.0f, 0.0f, // (0, 0)       
    1.0f, 1.0f, 0.0f, // (1, 1)
};

module u32 quad_indices[] = {
    0, 1, 2,
    0, 1, 3,
};

void render_init()
{
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);	    
    glViewport(0,0, app_state.window_width, app_state.window_height);
    glEnable(GL_DEPTH_TEST);

    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#if DEBUG   
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        printf("Successfully created an OpenGL debug context.\n");
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(GLDebugOutput, 0);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
    }
#endif
    
    // Compile shaders
    register_shader("..\\src\\shaders\\default.glsl", "default");
    register_shader("..\\src\\shaders\\ui.glsl", "ui");    
    register_shader("..\\src\\shaders\\cube.glsl", "cube");
    register_shader("..\\src\\shaders\\light.glsl", "light");
    
    if(!init_shader_bank())
    {
        printf("Failed init of shader bank!\n");
        exit(0);
    }
    
    ArenaMemory mesh_memory;
    {
        size_t region_size = MB(10);
        InitArena(&region1, ALLOC_MEM(region_size), region_size);

        region_size = GB(1);
        InitArena(&mesh_memory, ALLOC_MEM(region_size), region_size);
        
        region_size = MB(256);
        InitArena(&scratch_memory, ALLOC_MEM(region_size), region_size);        
    }

    test_model = LoadModelFromAssimp(&mesh_memory, &scratch_memory, "sponza", "sponza.obj");

#if 0
    
    // UI  
    glGenVertexArrays(1, &ui_VAO);
    glBindVertexArray(ui_VAO);

    glGenBuffers(1, &ui_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, ui_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vert), quad_vert, GL_STATIC_DRAW);

    glGenBuffers(1, &ui_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ui_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);      

#endif
    
    light_pos = create_vec3(0.0f, 3.0f, 1.0f);
    light_color = create_vec3(1.0f, 1.0f, 1.0f);
    light_dir = normalize_vec3(create_vec3(0.3f, -1.0f, 0.));
    
    vec3 light_amb = create_vec3(0.1f, 0.1f, 0.1f);
    vec3 light_diff = create_vec3(0.5f, 0.5f, 0.5f);
    vec3 light_spec = create_vec3(0.5f, 0.5f, 0.5f);

    use_program_name("default");
    
    set_int("diffuse_map", 0);
    set_int("specular_map", 1);
    set_int("ambient_map", 2);
    
    set_vec3f("dir_light.ambient", light_amb);
    set_vec3f("dir_light.diffuse", light_diff);
    set_vec3f("dir_light.specular", light_spec);
    
    vec3 cam_pos, cam_dir, cam_up;
    
    cam_pos = create_vec3(0.0f, 0.0f, 2.0f);
    cam_dir = create_vec3(0.0f, 0.0f, -1.0f);
    cam_up = create_vec3(0.0f, 1.0f, 0.0f);
        
    create_camera(&global_cam, cam_pos, cam_dir, cam_up, app_state.fov, 1000.0f);

}

void render(float dt)
{
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Model-view-projection */
    mat4x4 model, view, projection;
    vec3 rotation_axis, trans_vec;

    // Calculate perspective projection matrix
    projection = perspective(RADIANS(global_cam.fov), (float)app_state.window_width/(float)app_state.window_height, 0.1f, 5000.0f);
    
#if 0
    /* Translate scene forward */         
    create_vec3(0.0f, 0.0f, -3.0f);                        
    view = translate_mat4x4(view, transl_vec);
#endif

    view = get_camera_view_matrix(&global_cam);

    use_program_name("default");
    model = create_diag_mat4x4(1.0f);
    model = scale_mat4x4(model, 1.0f, 1.0f, 1.0f);
    set_mat4f("model", model.matrix);
    set_mat4f("view", view.matrix);
    set_mat4f("projection", projection.matrix);

    set_vec3f("view_pos", global_cam.position);
    float time = glfwGetTime();
    set_vec3f("dir_light.direction", light_dir);

      
    for(u32 mesh_index = 0; mesh_index < test_model.mesh_count; mesh_index++)
    {
        Mesh mesh = test_model.meshes[mesh_index];
        Material mat = mesh.material;

        glActiveTexture(GL_TEXTURE0);                
        glBindTexture(GL_TEXTURE_2D, mat.diffuse_map.id);

        glActiveTexture(GL_TEXTURE1);                
        glBindTexture(GL_TEXTURE_2D, mat.specular_map.id);

        glActiveTexture(GL_TEXTURE2);                
        glBindTexture(GL_TEXTURE_2D, mat.ambient_map.id);
        
        set_vec3f("material.ambient", mat.ambient);
        set_vec3f("material.diffuse", mat.diffuse);
        set_vec3f("material.specular", mat.specular);
        set_float("material.shininess", mat.shininess);                
        
        BindVertArr(mesh.va);
        
        glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, 0);
    }
    
        
#if 0        
    // UI
    {        
        use_program_name("ui");
        
        mat4x4 ortho_proj;

        create_diag_m4(model, 1.0f);
        create_diag_m4(ortho_proj, 1.0f);
        ortho(ortho_proj, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f);

        // Size and scaling
        float rect_width = 500.0f;
        float rect_height = 200.0f;            

        float rect_width_ndc = rect_width / app_state.window_width;
        float rect_height_ndc = rect_height / app_state.window_height;        

        scale_mat4x4(model, rect_width_ndc, rect_height_ndc, 0.0f);

        // Positioning
        vec3 rect_pos = create_vec3(0.5f - rect_width_ndc * 0.5f, 0.5f - rect_height_ndc * 0.5f, 0.0f);

        model = translate_mat4x4(model, &rect_pos);

        set_mat4f("model", model.matrix);
        set_mat4f("projection", ortho_proj.matrix);
        set_vec3f("color", 0.0f, 255.0f, 0.0f);
        set_float("scale", 0.5f);
        
        glBindVertexArray(ui_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
#endif
    
}
