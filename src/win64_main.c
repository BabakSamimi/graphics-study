__declspec(dllexport) int NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#include <stdio.h> // printf
#include <stdbool.h>
#include <math.h> // sin
#include <locale.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

/*
  TODO:
  * Cache glGetUniformLocation calls
  * Implement our own string struct
  * UTF-8
*/

// app code

#define GFX_MATH_IMPL
#include "gfx_math.h"

#include "win64_main.h"

#include "renderer/renderer.h"
#include "renderer/shader_bank.h"

#include "memory.h"

#define PRESSED(KEY) (glfwGetKey(window, KEY) == GLFW_PRESS)

AppState app_state;
extern ShaderBank shaders;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0,0, width, height);
}

module void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        switch(key)
        {
            case GLFW_KEY_ESCAPE:
            {
                
                if(app_state.camera_control)
                {
                    // Disable fps control and show cursor
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);  
                }
                else
                {
                    // Enable fps control and disable cursor
                    glfwGetCursorPos(window, &app_state.mouse_last_x, &app_state.mouse_last_y);
            
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
                }
        
                app_state.camera_control = !app_state.camera_control;
                
                break;
            }
            case GLFW_KEY_R:
            {
                if(!app_state.reloading_shaders)
                {
                    app_state.reloading_shaders = 1;
        
                    if(reload_shader_bank())
                    {
                        printf("Successfully reloaded shaders\n");
                    }
                    else
                        printf("Shader reloading failed\n");
        
                    app_state.reloading_shaders = 0;                     
                }

                break;
            }
            case GLFW_KEY_1:
            {
                app_state.wireframe_on = !app_state.wireframe_on;
                
                if(app_state.wireframe_on)
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                else
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
            }
            default: break;               
        }
    }

}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset)
{
    app_state.fov -= (float)y_offset;
    if (app_state.fov < 1.0f)
        app_state.fov = 1.0f;
    if (app_state.fov > 120.0f)
        app_state.fov = 120.0f;

    printf("New FOV Value: %.2f\n", app_state.fov);
}

s32 main(void)
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
    
#if DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif
    
    // Create a window and its OpenGL context
    app_state.window_width = 1280;
    app_state.window_height = 720;
    
    window = glfwCreateWindow(app_state.window_width, app_state.window_height, "graphics!", NULL, NULL);
	
	if (!window)
    {
        printf("Window creation failed\n");
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);	    
    glfwSwapInterval(1);
	
	// Register GLFW callbacks
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    render_init();

    // Print status
    {            
        printf("Vendor: %s, %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
        int major_ver = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
        int minor_ver = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
        printf("OpenGL version %d.%d\n", major_ver, minor_ver);        
    }        
        
    app_state.target_frame_ms = 1000.0f * (1.0f / 60.0f);
    app_state.delta_time = 0.0f;
    app_state.last_time = 0.0f;
    app_state.fov = 90.0f;

    app_state.mouse_x = 0.0f;
    app_state.mouse_y = 0.0f;
    app_state.mouse_last_x = (float)app_state.window_width / 2.0f;
    app_state.mouse_last_y = (float)app_state.window_height / 2.0f;

    app_state.sensitivity = 0.1f;
    app_state.camera_control = 1;
    app_state.wireframe_on = 0;

    vec3 cam_pos, cam_dir, cam_up;
    
    Camera cam;
    cam_pos = create_vec3(0.0f, 0.0f, 5.0f);
    cam_dir = create_vec3(0.0f, 0.0f, -1.0f);
    cam_up = create_vec3(0.0f, 1.0f, 0.0f);
        
    create_camera(&cam, cam_pos, cam_dir, cam_up, app_state.fov, 6.0f);        
    
    f64 reload_time = glfwGetTime();
    s32 frames_elapsed = 0;
        
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        f32 start_time = glfwGetTime();
        
        /* Poll for and process events */
        glfwPollEvents();
        
        if(app_state.camera_control)
        {
            glfwGetCursorPos(window, &app_state.mouse_x, &app_state.mouse_y);
        
            f32 x_offset = (app_state.mouse_x - app_state.mouse_last_x) * app_state.sensitivity;
            f32 y_offset = (app_state.mouse_last_y - app_state.mouse_y) * app_state.sensitivity;

            update_camera_transform(&cam, x_offset, y_offset);

            app_state.mouse_last_x = app_state.mouse_x;
            app_state.mouse_last_y = app_state.mouse_y;            
            cam.fov = app_state.fov;

        }            
        
        f32 walking_speed = cam.speed * app_state.delta_time;
        if(PRESSED(GLFW_KEY_W))
            move_camera(&cam, FORWARD, walking_speed);            
        if(PRESSED(GLFW_KEY_S))
            move_camera(&cam, BACKWARD, -walking_speed);            
        if(PRESSED(GLFW_KEY_A))
            move_camera(&cam, LEFT, walking_speed*0.8f);            
        if(PRESSED(GLFW_KEY_D))
            move_camera(&cam, RIGHT, -walking_speed*0.8f);

        render(app_state.delta_time*1000.0f, &cam);
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
        app_state.delta_time = start_time - app_state.last_time;    
        app_state.last_time = start_time;
        
        f64 end_time = glfwGetTime();
        f64 elapsed = end_time - reload_time;
        
        if(elapsed >= 1.0f)
        {
            reload_shader_bank();
            reload_time = end_time;
        }
        
        f64 work_time = app_state.delta_time*1000.0f;
        // printf("Frame work time: %0.4f ms\n", work_time);
        
        if(work_time > app_state.target_frame_ms)
        {
            // Insert sleep function here
        }

    }
	
    glfwTerminate();
    return 0;
}
