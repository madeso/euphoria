#include "render/debuggl.h"

#include "render/gl.h"
#include "assert/assert.h"
#include "log/log.h"


namespace
{
    std::string
    source_to_string(GLenum source)
    {
        switch(source)
        {
        case GL_DEBUG_SOURCE_API_ARB: return "API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB: return "Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: return "Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY_ARB: return "Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION_ARB: return "Application"; break;
        case GL_DEBUG_SOURCE_OTHER_ARB: return "Other"; break;
        default: return "Unknown";
        }
    }

    std::string
    type_to_string(GLenum type)
    {
        switch(type)
        {
        case GL_DEBUG_TYPE_ERROR_ARB: return "Error";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: return "Deprecated Behaviour";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB: return "Undefined Behaviour";
        case GL_DEBUG_TYPE_PORTABILITY_ARB: return "Portability";
        case GL_DEBUG_TYPE_PERFORMANCE_ARB: return "Performance";
        case GL_DEBUG_TYPE_OTHER_ARB: return "Other";
        default: return "Unknown";
        }
    }

    std::string
    severity_to_string(GLenum severity)
    {
        switch(severity)
        {
        case GL_DEBUG_SEVERITY_HIGH_ARB: return "high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM_ARB: return "medium"; break;
        case GL_DEBUG_SEVERITY_LOW_ARB: return "low"; break;
        default: return "unknown";
        }
    }
}



namespace euphoria::render
{
    std::string
    opengl_error_to_string(GLenum error_code)
    {
        switch(error_code)
        {
        case GL_INVALID_ENUM: return "INVALID_ENUM";
        case GL_INVALID_VALUE: return "INVALID_VALUE";
        case GL_INVALID_OPERATION: return "INVALID_OPERATION";
#ifdef GL_STACK_OVERFLOW
        case GL_STACK_OVERFLOW: return "STACK_OVERFLOW";
#endif
#ifdef GL_STACK_UNDERFLOW
        case GL_STACK_UNDERFLOW: return "STACK_UNDERFLOW";
#endif
        case GL_OUT_OF_MEMORY: return "OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "INVALID_FRAMEBUFFER_OPERATION";
        default: return "UNKNOWN";
        }
    }


    void
    print_all_opengl_errors(const char* file, int line)
    {
        GLenum error_code = 0;
        while((error_code = glGetError()) != GL_NO_ERROR)
        {
            const std::string error = opengl_error_to_string(error_code);
            LOG_ERROR("{0} | {1}({2})", error, file, line);
        }
    }


    void APIENTRY
    on_opengl_error
    (
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei /*length*/,
        const GLchar* message,
        const GLvoid* /*userParam*/
    )
    {
        // ignore non-significant error/warning codes
        if(type == GL_DEBUG_TYPE_OTHER_ARB)
        {
            return;
        }

        // only display the first 10
        static int error_count = 0;
        if(error_count > 10)
        {
            return;
        }
        ++error_count;

        LOG_ERROR("---------------");
        LOG_ERROR("Debug message ({0}): {1}", id, message);
        LOG_ERROR
        (
            "Source {0} type: {1} Severity: {2}",
            source_to_string(source),
            type_to_string(type),
            severity_to_string(severity)
        );
        // ASSERT(false);
    }


    void
    setup_opengl_debug()
    {
        const bool has_debug = GLAD_GL_ARB_debug_output == 1;
        if(has_debug)
        {
            LOG_INFO("Enabling OpenGL debug output");
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
            glDebugMessageCallbackARB(on_opengl_error, nullptr);
            glDebugMessageControlARB
            (
                GL_DONT_CARE,
                GL_DONT_CARE,
                GL_DONT_CARE,
                0,
                nullptr,
                GL_TRUE
            );
        }
    }
}
