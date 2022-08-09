#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "shader_bank.h"
#include "renderer.h"

#include "cube.h"

#include "..\win64_main.h"
#include "..\defines.h"
#include "..\gfx_math.h"
#include "..\memory.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

module void APIENTRY GLDebugOutput(GLenum source, GLenum type, GLuint id,
                              GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    
    char *source_str, *type_str, *severity_str;


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

unsigned int load_texture(char* path, int flipped)
{
    // Setup Texture
    unsigned int texture;
    glGenTextures(1, &texture);

    // Load our texture
    int tex_width, tex_height, nr_channels;
    unsigned char* tex_data = stbi_load(path, &tex_width, &tex_height, &nr_channels, 0);
    
    if(tex_data)
    {
        GLenum format;
        
        if (nr_channels == 3)
            format = GL_RGB;
        else if (nr_channels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, texture);
        
        glTexImage2D(GL_TEXTURE_2D, 0, format, tex_width, tex_height, 0, format, GL_UNSIGNED_BYTE, tex_data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glBindTexture(GL_TEXTURE_2D, 0);    
        stbi_set_flip_vertically_on_load(flipped); // this image starts at top left            
        stbi_image_free(tex_data);
        
        printf("Loaded texture: %s\n", path);
    }
    else printf("Texture was not loaded.\n");
    
    return texture;
}

// unit square, first quadrant of NDC
module float quad_vert[] = {
    0.0f, 1.0f, 0.0f, // (0, 1)
    1.0f, 0.0f, 0.0f, // (1, 0)

    0.0f, 0.0f, 0.0f, // (0, 0)       
    1.0f, 1.0f, 0.0f, // (1, 1)
};

module unsigned int quad_indices[] = {
    0, 1, 2,
    0, 1, 3,
};
    
// Index Buffer
module unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};

#define box_padding 2.0f
module vec3 box_positions[6] = {
    {0}, {-1.0f - box_padding, 0.0f, 0.0f}, {1.0f + box_padding, 0.0f, 0.0f},
    {0.0f, 1.0f + box_padding, 0.0f},
    {0.0f, 0.0f, -1.0f - box_padding}, {0.0f, 0.0f, 1.0f + box_padding}
};

module vec3 pointlight_positions[4][2] = {
    {
        { 4.0f, 4.0f, 0.0f }, // pos
        { 1.0f, 1.0f, 0.0f }  // color
    },
    {
        { -4.0f, 4.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f }
    },
    {
        { 0.0f, 4.0f, 4.0f },
        { 0.0f, 1.0f, 0.0f }
    },
    {
        { 0.0f, 4.0f, -4.0f },
        { 0.0f, 0.0f, 1.0f }
    },        
};

module vec3 light_pos;
module vec3 light_color;

// Our primary VA for cube rendering
module VertexArray va;
module VertexArray light_va;

module MemoryRegion region1;

module int diffuse_map;
module int specular_map;
module int emission_map;

extern AppState app_state;
extern ShaderBank shaders;

void render_init()
{
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
    register_shader("..\\src\\shaders\\cube.glsl", "cube");
    register_shader("..\\src\\shaders\\light.glsl", "light");
    register_shader("..\\src\\shaders\\ui.glsl", "ui");
    
    if(!init_shader_bank())
    {
        printf("Failed init of shader bank!\n");
        exit(0);
    }

    {
        size_t region_size = MB(1);
        InitRegion(&region1, ALLOC_MEM(region_size), region_size);
    }

    VertexLayout va_layout = {0};
    VertexBuffer cube_vb;

    // Cube VA
    va = GenVertArr();    
    cube_vb = GenVertBuf(cube_vertices, sizeof(cube_vertices));
                
    va_layout.attributes = (VertexAttribute*)SliceRegion16(&region1, 3 * sizeof(VertexAttribute));
    
    VertLayoutPush(&va_layout, 3, GL_FLOAT, GL_FALSE);
    VertLayoutPush(&va_layout, 3, GL_FLOAT, GL_FALSE);
    VertLayoutPush(&va_layout, 2, GL_FLOAT, GL_FALSE);

    BindVertArr(va);               
    BindVertBuf(cube_vb);
    VABindLayout(&va, va_layout);
    
    UnbindVertArr();
    UnbindVertBuf();
    
    // Light cubes VA
    light_va = GenVertArr();    
    VertexLayout light_layout = {0};
    
    //light_layout.attributes = (VertexAttribute*)SliceRegion16(&region1, 1 * sizeof(VertexAttribute));
    
    //VertLayoutPush(&light_layout, 3, GL_FLOAT, GL_FALSE);
    
    BindVertArr(light_va);    
    BindVertBuf(cube_vb);
    VABindLayout(&light_va, va_layout); // our light cube happens to have the same VA Layout

    UnbindVertArr();
    UnbindVertBuf();

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
    
    /* Setup EBO
       glGenBuffers(1, &EBO);
       glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
       glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);    

       glBindVertexArray(0);
       glBindBuffer(GL_ARRAY_BUFFER, 0);
    */

    diffuse_map = load_texture("assets\\container2.png", 1);
    specular_map = load_texture("assets\\container2_specular.png", 1);
    emission_map = load_texture("assets\\container2_emission.jpg", 0);    

    // Update texture units in our fragment shader
    use_program_name("cube");
    set_int("material.diffuse", 0);
    set_int("material.specular", 1);
    set_int("material.emission", 2);    

    light_pos = create_vec3(0.0f, 0.0f, 0.0f);
    light_color = create_vec3(0.0f, 1.0f, 1.0f);

}

void render(float dt, Camera *cam)
{
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Model-view-projection */
    mat4x4 model, view, projection;
    vec3 rotation_axis, trans_vec;

    // Calculate perspective projection matrix
    projection = perspective(RADIANS(cam->fov), (float)app_state.window_width/(float)app_state.window_height, 0.1f, 100.0f);        
        
#if 0
    /* Translate scene forward */         
    create_vec3(0.0f, 0.0f, -3.0f);                        
    view = translate_mat4x4(view, transl_vec);
#endif

    view = get_camera_view_matrix(cam);        

    /* Render cube */
    use_program_name("cube");

    set_mat4f("view", view.matrix);
    set_mat4f("projection", projection.matrix);                         

    // bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuse_map);
        
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specular_map);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, emission_map);

    /* Update uniforms  */
    float time_value = glfwGetTime();

#if 0       
    light_pos.x = 2.0f * sin(time_value) * cos(time_value);
    light_pos.y = 2.0f * sin(time_value) * sin(time_value);
    light_pos.z = -2.0f; //2.0f * cos(time_value);

    light_color.x = (sin(time_value) + 1) / 2;
    light_color.y = (cos(time_value) + 1) / 2;
    light_color.z = 0.7 * ((sin(time_value) + 1) / 2);
#else
    light_pos.x = 0.0f;
    light_pos.y = 1.0f;
    light_pos.z = 5.0f;

    light_color.x = 1.0f;
    light_color.y = 1.0f;
    light_color.z = 1.0f;
#endif
        
    //set_float("u_time", time_value);        
    set_vec3f("cam_pos", cam->position);

    set_float("material.shininess", 32.0f);

    //set_vec3f("light.direction", -0.2f, 0.3f, -0.3f);
        
    // Set directional light
    {        
        set_vec3f("dir_light.direction", cam->direction);
        set_vec3f("dir_light.phong.ambient", create_vec3(0.001f, 0.001f, 0.001f));
        set_vec3f("dir_light.phong.diffuse", create_vec3(0.1f, 0.1f, 0.1f));
        set_vec3f("dir_light.phong.specular", create_vec3(0.5f, 0.5f, 0.5f));
    }
        
    // Set pointlights
    {
        float phong_coeff = 0.005f;
        
        vec3 pos = pointlight_positions[0][0];
        vec3 colors = pointlight_positions[0][1];
        set_float("pointLights[0].power", 20.0f);
        set_vec3f("pointLights[0].position",        pos);
        set_vec3f("pointLights[0].phong.ambient",   scale_vec3(colors, phong_coeff));
        set_vec3f("pointLights[0].phong.diffuse",   colors);
        set_vec3f("pointLights[0].phong.specular",  colors);

        pos = pointlight_positions[1][0];
        colors = pointlight_positions[1][1];
        set_float("pointLights[1].power", 2.0f + 5.0f);
        set_vec3f("pointLights[1].position",        pos);
        set_vec3f("pointLights[1].phong.ambient",   scale_vec3(colors, phong_coeff));
        set_vec3f("pointLights[1].phong.diffuse",   colors);
        set_vec3f("pointLights[1].phong.specular",  colors);
            
        pos = pointlight_positions[2][0];
        colors = pointlight_positions[2][1];
        set_float("pointLights[2].power", 4.0f + 5.0f);
        set_vec3f("pointLights[2].position",        pos);
        set_vec3f("pointLights[2].phong.ambient",   scale_vec3(colors, phong_coeff));
        set_vec3f("pointLights[2].phong.diffuse",   colors);
        set_vec3f("pointLights[2].phong.specular",  colors);

        pos = pointlight_positions[3][0];
        colors = pointlight_positions[3][1];
        set_float("pointLights[3].power", 20.0f + 5.0f);
        set_vec3f("pointLights[3].position",        pos);
        set_vec3f("pointLights[3].phong.ambient",   scale_vec3(colors, phong_coeff));
        set_vec3f("pointLights[3].phong.diffuse",   colors);
        set_vec3f("pointLights[3].phong.specular",  colors);
            
    }
        
    // Set spotlight
        
    {

        set_vec3f("spotlight.position", cam->position);
        set_vec3f("spotlight.direction", cam->direction);            
        set_float("spotlight.power", 15.0f);
        set_float("spotlight.cutoff", (float)cos(RADIANS(15.0f)));
        set_float("spotlight.outer_cutoff", (float)cos(RADIANS(18.0f)));

        float intensity = 0.1f;
        set_vec3f("spotlight.phong.ambient", create_vec3(0.0f, 0.0f, 0.0f));
        set_vec3f("spotlight.phong.diffuse", create_vec3(1.0f, 1.0f, 1.0f)); // color of light
        set_vec3f("spotlight.phong.specular", create_vec3(1.0f, 1.0f, 1.0f));        
    }
        
    // Rotate in model space        
           //create_vec3(&rotation_axis, 1.0f, 0.0f, 0.0f);
           //rotate_mat4x4(model, (float)sin(0.25 * (float)glfwGetTime() * RADIANS(90.0f)), &rotation_axis);               


#if 1
    for(unsigned int box_index = 0; box_index < 6; box_index++)
    {
        model = create_diag_mat4x4(1.0f);
        rotation_axis = create_vec3(0.0f, 1.0f, 0.0f);
            
        model = scale_mat4x4(model, 1.0f, 2.0f, 1.0f);
        model = rotate_mat4x4(model, (float)glfwGetTime(), rotation_axis);
        model = translate_mat4x4(model, box_positions[box_index]);
            
        set_mat4f("model", model.matrix);
            
        BindVertArr(va);
        glDrawArrays(GL_TRIANGLES, 0, 36);
            
    }
#else
    model = create_diag_mat4x4(1.0f);
    set_mat4f("model", model.matrix);
        
    /* Draw cube */
    BindVertArr(va);                       
    glDrawArrays(GL_TRIANGLES, 0, 36);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw using our index buffer
    
#endif
        
    // Render light cubes
#if 1       
    use_program_name("light");        

    for(int i = 0; i < 4; i++)
    {
        vec3 pos = pointlight_positions[i][0];
        vec3 color = pointlight_positions[i][1];
        vec3 rotation_axis = create_vec3(0.0f, 1.0f, 0.0f);
        model = create_diag_mat4x4(1.0f);
            
        model = scale_mat4x4(model, 3.0f, 0.2f, 0.2f);            
        model = rotate_mat4x4(model, (float)glfwGetTime(), rotation_axis);
        model = translate_mat4x4(model, pos);

        set_vec3f("light_color", color);

        set_mat4f("model", model.matrix);
        set_mat4f("view", view.matrix);
        set_mat4f("projection", projection.matrix);
        
        BindVertArr(light_va);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }    
                       
#endif
        
#if 0        
    // UI
    {        
        use_program_name("ui");
        
        mat4 ortho_proj;

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
