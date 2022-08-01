#include "glad/glad.h"
#include "renderer.h"
#include "vertex_buffer.h"
#include "index_buffer.h"


// @REWORK: Use switch cases instead of this?
static char* gl_debug_source_strings[6] = {
    "OpenGL API", "Window system", "Shader compiler",
    "Third party", "Application", "Other",
};

static char* gl_debug_type_strings[8] = {
    "OpenGL API", "Deprecated behaviour", "Undefined behaviour",
    "Portability", "Performance",
    "Marker", "Push group", "Pop group",
};

static char* gl_debug_severity_strings[4] = {
    "High", "Medium", "Low", "Notification",
};

/*
  length:
      is the length of the message excluding null-terminator
  
  source, type and severity:
      are the message's enumerators
  
  id:
      is the message's identifier
  
  userParam:
      is for storing custom data by passing in a struct, this is done via glDebugMessageCallback(...)
*/

static void APIENTRY gl_debug_output(GLenum source, GLenum type, GLuint id,
                              GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    
    char* source_str = gl_debug_source_strings[(ArrayCount(gl_debug_source_strings) - ((GL_DEBUG_SOURCE_OTHER + 1) - source)) % ArrayCount(gl_debug_source_strings)];


    // @REWORK: Have to redo this and just use simple switches
    char* type_str;
    if(type <= GL_DEBUG_TYPE_OTHER)
    {
        type_str = gl_debug_type_strings[(ArrayCount(gl_debug_type_strings)- 2 - ((GL_DEBUG_TYPE_OTHER + 1) - type)) % ArrayCount(gl_debug_type_strings)];
    }
    else
    {
        type_str = gl_debug_type_strings[(ArrayCount(gl_debug_type_strings) - 5 - ((GL_DEBUG_TYPE_POP_GROUP + 1) - type)) % ArrayCount(gl_debug_type_strings)];
    }

    char* severity_str;
    if(severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    {
        severity_str = gl_debug_severity_strings[3];
    }
    else
    {
        severity_str = gl_debug_severity_strings[(ArrayCount(gl_debug_severity_strings) - ((GL_DEBUG_SEVERITY_LOW + 1) - severity)) % ArrayCount(gl_debug_severity_strings)];        
    }

    
    printf("[OpenGL] (%d) [Source: %s] [Type: %s] [Severity: %s]:\n %s\n\n", id, source_str, type_str, severity_str, message);
    
}

void RenderInit(int window_width, int window_height)
{
    glViewport(0,0, window_width, window_height);
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
        glDebugMessageCallback(gl_debug_output, 0);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
    }
#endif
        

}

void Render(float dt)
{
    
}
