__declspec(dllexport) int NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#include <stdio.h> // printf
#include <stdbool.h> // true, false
#include <math.h> // sin

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <stdint.h>
typedef int8_t s8;
typedef uint8_t u8;

typedef int16_t s16;
typedef uint16_t u16;

typedef int32_t s32;
typedef uint32_t u32;

typedef int64_t s64;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

#define ArrayCount(A) (sizeof((A)) / sizeof((A)[0]))

int WIDTH = 1280;
int HEIGHT = 720;
float FOV = 90.0f;

// app code
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shader_bank.h"

#define GFX_MATH_IMPL
#include "gfx_math.h"

#include "camera.h"

#define PRESSED(KEY) (glfwGetKey(window, KEY) == GLFW_PRESS)

typedef struct
{
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
    if (FOV < 1.0f)
        FOV = 1.0f;
    if (FOV > 120.0f)
        FOV = 120.0f;

    printf("New FOV Value: %.2f\n", state.fov);
}

int main(void)
{
    GLFWwindow* window;
    
#if DEBUG
    printf("Debug mode on\n");
#endif
    
    /* Initialize the library */
    if (!glfwInit())
        return -1;
	
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	
    /* Create a window and its OpenGL context */
    window = glfwCreateWindow(WIDTH, HEIGHT, "learnopengl", NULL, NULL);
	
	if (!window)
    {
        printf("Window creation failed\n");
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);    
    
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	
	glViewport(0,0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(1);
	
	/* Register GLFW callbacks */
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);    
  
    printf("Vendor: %s, %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
	int major_ver = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    int minor_ver = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
	printf("OpenGL version %d.%d\n", major_ver, minor_ver);

    /* Vertex Buffer */
#if 0     
    float vertices[] = {
        // positions        // colors        // texture coordinates
        0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // top left 
    };
#endif
    
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };    
    
    /* Index Buffer */    
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };  
    
    init_shader_bank();
    
    // Create a VAO to store the layout of our attributes
    u32 VBO, VAO, EBO, texture0, texture1;

    // Setup VAO
    glGenVertexArrays(1, &VAO);    
    glBindVertexArray(VAO);

    // Setup VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Setup vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(f32), (void*)(3*sizeof(f32)));
    glEnableVertexAttribArray(1);

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

    s32 tex_width, tex_height, nr_channels;
    u8* tex_data = stbi_load("bin\\assets\\container.jpg", &tex_width, &tex_height, &nr_channels, 0);
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

    state.delta_time = 0.0f;
    state.last_frame = 0.0f;
    state.fov = 90.0f;

    state.mouse_x = 0.0f;
    state.mouse_y = 0.0f;
    state.mouse_last_x = (float)WIDTH / 2.0f;
    state.mouse_last_y = (float)HEIGHT / 2.0f;

    Camera cam;
    {    
        vec3 cam_pos, cam_dir, cam_up;

        init_v3(&cam_pos, 0.0f, 0.0f, 5.0f);
        init_v3(&cam_dir, 0.0f, 0.0f, -1.0f);
        init_v3(&cam_up, 0.0f, 1.0f, 0.0f);
        
        init_camera(&cam, &cam_pos, &cam_dir, &cam_up, state.fov, 0.1f, 3.0f);        
    }
    
    f64 start_time = glfwGetTime();    
        
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        float current_frame = glfwGetTime();
        state.delta_time = current_frame - state.last_frame;
        state.last_frame = current_frame;
        
		process_input(window);
        glfwGetCursorPos(window, &state.mouse_x, &state.mouse_y);
        
        float x_offset = (state.mouse_x - state.mouse_last_x) * cam.sensitivity;
        float y_offset = (state.mouse_last_y - state.mouse_y) * cam.sensitivity;
        
        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* bind appropiate shaders */
        use_program_name("cube");

        /* update uniform */
        {
            f32 time_value = glfwGetTime();
            set_float("u_time", time_value);

            mat4 model, view;
            vec3 rotation_axis, trans_vec;
                            
            /* Rotate in model space */
            init_diag_m4(model, 1.0f);              
            init_v3(&rotation_axis, 1.0f, 0.0f, 0.0f);
            //rotate_m4(model, (float)sin(0.25 * (float)glfwGetTime() * RADIANS(90.0f)), &rotation_axis);
            
            /* Translate scene forward */     
            init_diag_m4(view, 1.0f);
#if 0            
            init_v3(&trans_vec, 0.0f, 0.0f, -3.0f);                        
            translate_m4(view, &trans_vec);
#endif

            float cam_speed = cam.speed * state.delta_time;            

            cam.yaw -= x_offset;
            cam.pitch += y_offset;
            if (cam.pitch > 89.0f) cam.pitch = 89.0f;
            if (cam.pitch < -89.0f) cam.pitch = -89.0f;
            cam.fov = state.fov;
            
            vec3 dir;
            dir.x = cos(RADIANS(cam.yaw)) * cos(RADIANS(cam.pitch));
            dir.y = sin(RADIANS(cam.pitch));
            dir.z = sin(RADIANS(cam.yaw)) * cos(RADIANS(cam.pitch));
            normalize_v3(&dir);
            copy_v3(&cam.direction, &dir);           
            
            if(PRESSED(GLFW_KEY_W))
            {
                vec3 temp;
                copy_v3(&temp, &cam.direction);
                scale_v3(&temp, cam_speed);
                add_v3(&cam.position, &cam.position, &temp);
            }
            if(PRESSED(GLFW_KEY_S))
            {
                vec3 temp;                
                copy_v3(&temp, &cam.direction);
                scale_v3(&temp, cam_speed);
                
                sub_v3(&cam.position, &cam.position, &temp);
            }
            if(PRESSED(GLFW_KEY_A))
            {
                vec3 temp;
                
                cross_v3(&temp, &cam.direction, &cam.up);
                normalize_v3(&temp);
                scale_v3(&temp, cam_speed);               
                
                add_v3(&cam.position, &cam.position, &temp);
            }
            if(PRESSED(GLFW_KEY_D))
            {
                vec3 temp;
                
                cross_v3(&temp, &cam.direction, &cam.up);
                normalize_v3(&temp);
                scale_v3(&temp, cam_speed);                
                
                sub_v3(&cam.position, &cam.position, &temp);
            }            
            
            vec3 cam_look_at;
            add_v3(&cam_look_at, &cam.position, &cam.direction);
            
            look_at(view, &cam.position, &cam_look_at, &cam.up);

            /* Projection */
            mat4 projection;
            perspective(projection, RADIANS(cam.fov), (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);

            set_mat4f("model", model);
            set_mat4f("view", view);
            set_mat4f("projection", projection);
            
        }        
        
        /* bind textures */
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture1);        
        
        /* draw quad */
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw using our index buffer
		
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
		
        /* Poll for and process events */
        glfwPollEvents();

        f64 end_time = glfwGetTime();
        f64 elapsed = end_time - start_time;
        
        if(elapsed > 1.0f)
        {
            reload_shader_bank();
            start_time = glfwGetTime();
        }

        state.mouse_last_x = state.mouse_x;
        state.mouse_last_y = state.mouse_y;
    }
	
    glfwTerminate();
    return 0;
}
