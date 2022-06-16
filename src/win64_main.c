__declspec(dllexport) int NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#include <stdio.h> // printf
#include <stdbool.h> // true, false
#include <math.h> // sin

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define ArrayCount(A) (sizeof((A)) / sizeof((A)[0]))

int window_width = 1280;
int window_height = 720;

// app code
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define GFX_MATH_IMPL
#include "gfx_math.h"

#include "shader_bank.h"
#include "camera.h"

#define PRESSED(KEY) (glfwGetKey(window, KEY) == GLFW_PRESS)

typedef struct
{
    float target_frame_ms;
    float delta_time;
    float last_frame;
    float fov;
    
    double mouse_x;
    double mouse_last_x;
    double mouse_y;
    double mouse_last_y;

    int reloading_shaders;
    
} AppState;

AppState state;
extern ShaderBank shaders;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0,0, width, height);
}

void process_input(GLFWwindow* window)
{
	if(PRESSED(GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, true);
	else if(!state.reloading_shaders && PRESSED(GLFW_KEY_R))
    {
        state.reloading_shaders = 1;
        printf("R!\n");
        reload_shader_bank();
        state.reloading_shaders = 0;
    }
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset)
{
    state.fov -= (float)y_offset;
    if (state.fov < 1.0f)
        state.fov = 1.0f;
    if (state.fov > 120.0f)
        state.fov = 120.0f;

    printf("New FOV Value: %.2f\n", state.fov);
}

int main(void)
{
    GLFWwindow* window;
    
#if DEBUG
    printf("Debug mode on\n");
#endif
    
    // Initialize the library
    if (!glfwInit())
        return -1;
	
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	
    // Create a window and its OpenGL context
    window = glfwCreateWindow(window_width, window_height, "learnopengl", NULL, NULL);
	
	if (!window)
    {
        printf("Window creation failed\n");
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);    
    
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	
	glViewport(0,0, window_width, window_height);
    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(1);
	
	// Register GLFW callbacks
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);    
  
    printf("Vendor: %s, %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
	int major_ver = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    int minor_ver = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
	printf("OpenGL version %d.%d\n", major_ver, minor_ver);

    // Vertex Buffer
#if 0     
    float vertices[] = {
        // positions        // colors        // texture coordinates
        0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // top left 
    };
#endif
    

    // Cube with vertices and normals
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    
    // Index Buffer
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };  
    
    init_shader_bank();
    
    // Create a VAO to store the layout of our attributes
    unsigned int VBO, VAO, EBO, texture0, texture1;
    unsigned int light_VAO;
    
    // Setup VAO
    glGenVertexArrays(1, &VAO);    
    glBindVertexArray(VAO);

    // Setup VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Setup vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);    

    // Setup light cube
    glGenVertexArrays(1, &light_VAO);    
    glBindVertexArray(light_VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // re-use the same VBO
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);    

    /*
    // Setup EBO
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);    

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Setup Texture    
    glGenTextures(1, &texture0);
    glBindTexture(GL_TEXTURE_2D, texture0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int tex_width, tex_height, nr_channels;
    unsigned char* tex_data = stbi_load("bin\\assets\\container.jpg", &tex_width, &tex_height, &nr_channels, 0);
    if(tex_data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else printf("Texture was not loaded.\n");

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true); // this image starts at top left
    tex_data = stbi_load("bin\\assets\\dices.png", &tex_width, &tex_height, &nr_channels, 0);
    if(tex_data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else printf("Texture was not loaded.\n");
    
    stbi_image_free(tex_data);

    // Update texture units in our fragment shader
    use_program_name("cube");
    set_int("texture0", 0);
    set_int("texture1", 1);

    // bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    
    */
    
    state.target_frame_ms = 1000.0f * (1.0f / 60.0f);
    state.delta_time = 0.0f;
    state.last_frame = 0.0f;
    state.fov = 90.0f;

    state.mouse_x = 0.0f;
    state.mouse_y = 0.0f;
    state.mouse_last_x = (float)window_width / 2.0f;
    state.mouse_last_y = (float)window_height / 2.0f;

    Camera cam;
    {    
        vec3 cam_pos, cam_dir, cam_up;

        init_v3(&cam_pos, 0.0f, 0.0f, 5.0f);
        init_v3(&cam_dir, 0.0f, 0.0f, -1.0f);
        init_v3(&cam_up, 0.0f, 1.0f, 0.0f);
        
        init_camera(&cam, &cam_pos, &cam_dir, &cam_up, state.fov, 0.1f, 4.0f);        
    }

    vec3 light_pos;
    init_v3(&light_pos, -2.0f, 0.5f, 1.0f);
    
    double reload_time = glfwGetTime();
    int frames_elapsed = 0;
        
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        float current_frame = glfwGetTime();
        
		process_input(window);
        glfwGetCursorPos(window, &state.mouse_x, &state.mouse_y);
        
        float x_offset = (state.mouse_x - state.mouse_last_x) * cam.sensitivity;
        float y_offset = (state.mouse_last_y - state.mouse_y) * cam.sensitivity;

        update_camera_transform(&cam, x_offset, y_offset);        
        cam.fov = state.fov;
        
        float walking_speed = cam.speed * state.delta_time;                        
        if(PRESSED(GLFW_KEY_W))
            move_camera(&cam, FORWARD, walking_speed);            
        if(PRESSED(GLFW_KEY_S))
            move_camera(&cam, BACKWARD, -walking_speed);            
        if(PRESSED(GLFW_KEY_A))
            move_camera(&cam, LEFT, walking_speed*0.8f);            
        if(PRESSED(GLFW_KEY_D))
            move_camera(&cam, RIGHT, -walking_speed*0.8f);        
        
        /* Render starts here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Model-view-projection */
        mat4 model, view, projection;
        vec3 rotation_axis, trans_vec;

        // Calculate perspective projection matrix
        perspective(projection, RADIANS(cam.fov), (float)window_width/(float)window_height, 0.1f, 100.0f);
        
        init_diag_m4(view, 1.0f);
        
#if 0
        /* Translate scene forward */         
        init_v3(&trans_vec, 0.0f, 0.0f, -3.0f);                        
        translate_m4(view, &trans_vec);
#endif

        get_camera_view_matrix(view, &cam);        

        /* Render cube */
        use_program_name("cube");

        /* Update uniforms  */
        float time_value = glfwGetTime();
        
        //set_float("u_time", time_value);        
        set_vec3f("object_color", (float[]){0.4f, 0.2f, 0.6f});
        set_vec3f("light_color", (float[]){1.0f, 1.0f, 1.0f});
        set_vec3f("light_pos", (float[]){light_pos.x, light_pos.y, light_pos.z});
        set_vec3f("view_pos", (float[]){cam.position.x, cam.position.y, cam.position.z});
                            
        init_diag_m4(model, 1.0f);
        
        // Rotate in model space        
        //init_v3(&rotation_axis, 1.0f, 0.0f, 0.0f);
        //rotate_m4(model, (float)sin(0.25 * (float)glfwGetTime() * RADIANS(90.0f)), &rotation_axis);                

        set_mat4f("model", model);
        set_mat4f("view", view);
        set_mat4f("projection", projection);                 
        
        /* Draw cube */
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw using our index buffer

        // Render light cube
        use_program_name("light");
        light_pos.y = 0.5f * sin(time_value);
        init_diag_m4(model, 1.0f);        
        scale_m4(model, 0.2f, 0.2f, 0.2f);       
        translate_m4(model, &light_pos);
        
        set_mat4f("model", model);
        set_mat4f("view", view);
        set_mat4f("projection", projection);
        
        glBindVertexArray(light_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
		
        /* Poll for and process events */
        glfwPollEvents();
        
        double end_time = glfwGetTime();
        double elapsed = end_time - reload_time;
        
        if(elapsed >= 1.0f)
        {
            reload_shader_bank();
            reload_time = end_time;
        }

        state.mouse_last_x = state.mouse_x;
        state.mouse_last_y = state.mouse_y;
        
        state.delta_time = current_frame - state.last_frame;
        state.last_frame = current_frame;

        double work_time = state.delta_time*1000.0f;
        // printf("Frame work time: %0.4f ms\n", work_time);
        if(work_time > state.target_frame_ms)
        {
            /* Insert sleep function here when not using vsync */
        }

    }
	
    glfwTerminate();
    return 0;
}
