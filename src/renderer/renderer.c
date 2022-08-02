#include "glad/glad.h"
#include "renderer.h"
#include "vertex_buffer.h"
#include "index_buffer.h"

static void APIENTRY GLDebugOutput(GLenum source, GLenum type, GLuint id,
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
        glDebugMessageCallback(GLDebugOutput, 0);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
    }
#endif
        

}

void Render(float dt)
{
    
}
