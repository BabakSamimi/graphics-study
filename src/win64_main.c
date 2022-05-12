__declspec(dllexport) int NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

// CRT 
#include <stdio.h> // printf, FILE
#include <stdlib.h> // malloc, calloc
#include <stdbool.h> // true, false
#include <sys/stat.h> // stat
#include <time.h> // timespec
#include <string.h> // memset, strlen
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

int WIDTH = 800;
int HEIGHT = 600;

// app code
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shader_bank.c"

#define GFX_MATH_IMPL
#define GFX_GL
#include "gfx_math.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0,0, width, height);
}

s32 reloading_shaders = 0;

void process_input(GLFWwindow* window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	else if(!reloading_shaders && glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        reloading_shaders = 1;
        printf("R!\n");
        reload_shader_bank();
        reloading_shaders = 0;
    }
}

int main(void)
{
    GLFWwindow* window;
	
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
    glfwSwapInterval(1);
	
	/* Register GLFW callbacks */
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  
    printf("Vendor: %s, %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
	int major_ver = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    int minor_ver = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
	printf("OpenGL version %d.%d\n", major_ver, minor_ver);

    /* Vertex Buffer */
    float vertices[] = {
        // positions        // colors        // texture coordinates
        0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // top left 
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)(3*sizeof(f32)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)(6*sizeof(f32)));
    glEnableVertexAttribArray(2);

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
    glUseProgram(shaders.programs[0]);
    glUniform1i(glGetUniformLocation(shaders.programs[0], "texture0"), 0);
    glUniform1i(glGetUniformLocation(shaders.programs[0], "texture1"), 1);

    u32 transform_location = glGetUniformLocation(shaders.programs[0], "transform");    
    
    f64 start_time = glfwGetTime();
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
		
		process_input(window);
		
        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

        /* Bind appropiate shaders */
        glUseProgram(shaders.programs[0]);

        /* update uniform */
        {
            f32 time_value = glfwGetTime();
            s32 u_time_location = glGetUniformLocation(shaders.programs[0], "u_time");        
            glUniform1f(u_time_location, time_value);
            glUniform1i(glGetUniformLocation(shaders.programs[0], "texture0"), 0);
            glUniform1i(glGetUniformLocation(shaders.programs[0], "texture1"), 1);

            mat4 trans;
            vec3 rotation_axis, trans_vec;
    
            init_diag_m4(trans, 1.0f);
            init_v3(&rotation_axis, 0.0f, 0.0f, 1.0f);
            init_v3(&trans_vec, 0.5f, -0.5f, 0.0f);
            
            translate_m4(trans, &trans_vec);            
            rotate_m4(trans, (float)glfwGetTime(), &rotation_axis);
            glUniformMatrix4fv(transform_location, 1, GL_FALSE, trans);
        }
        
        
        /* bind textures */
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture1);

        /* draw quad */
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw using our index buffer

        /* update uniform */
        {

            mat4 trans;
            vec3 rotation_axis, trans_vec;
    
            init_diag_m4(trans, 1.0f);
            init_v3(&rotation_axis, 0.0f, 0.0f, 1.0f);
            init_v3(&trans_vec, -0.5f, 0.5f, 0.0f);
            
            scale_m4(trans, 0.5f, 1.33f, 1.0f);            
            translate_m4(trans, &trans_vec);            
            rotate_m4(trans, (float)glfwGetTime(), &rotation_axis);

            glUniformMatrix4fv(transform_location, 1, GL_FALSE, trans);
        }
        
        /* draw quad */
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Draw using our index buffer
		
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
        
    }
	
    glfwTerminate();
    return 0;
}
