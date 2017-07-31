#include "render/debuggl.h"

#include <iostream>
#include "render/gl.h"
#include "core/assert.h"

std::string OpenglErrorToString(GLenum errorCode) {
  switch (errorCode) {
    case GL_INVALID_ENUM:
      return "INVALID_ENUM";
      break;
    case GL_INVALID_VALUE:
      return "INVALID_VALUE";
      break;
    case GL_INVALID_OPERATION:
      return "INVALID_OPERATION";
      break;
#ifdef GL_STACK_OVERFLOW
    case GL_STACK_OVERFLOW:
      return "STACK_OVERFLOW";
      break;
#endif
#ifdef GL_STACK_UNDERFLOW
    case GL_STACK_UNDERFLOW:
      return "STACK_UNDERFLOW";
      break;
#endif
    case GL_OUT_OF_MEMORY:
      return "OUT_OF_MEMORY";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      return "INVALID_FRAMEBUFFER_OPERATION";
      break;
    default:
      return "UNKNOWN";
      break;
  }
}

void PrintAllOpenglErrors(const char *file, int line) {
  GLenum errorCode;
  while ((errorCode = glGetError()) != GL_NO_ERROR) {
    std::string error;
    error = OpenglErrorToString(errorCode);
    std::cout << error << " | " << file << " (" << line << ")" << std::endl;
  }
}

namespace {

std::string SourceToString(GLenum source) {
  switch (source) {
    case GL_DEBUG_SOURCE_API_ARB:
      return "API";
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
      return "Window System";
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
      return "Shader Compiler";
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
      return "Third Party";
      break;
    case GL_DEBUG_SOURCE_APPLICATION_ARB:
      return "Application";
      break;
    case GL_DEBUG_SOURCE_OTHER_ARB:
      return "Other";
      break;
    default:
      return "Unknown";
  }
}

std::string TypeToString(GLenum type) {
  switch (type) {
    case GL_DEBUG_TYPE_ERROR_ARB:
      return "Error";
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
      return "Deprecated Behaviour";
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
      return "Undefined Behaviour";
      break;
    case GL_DEBUG_TYPE_PORTABILITY_ARB:
      return "Portability";
      break;
    case GL_DEBUG_TYPE_PERFORMANCE_ARB:
      return "Performance";
      break;
    case GL_DEBUG_TYPE_OTHER_ARB:
      return "Other";
      break;
    default:
      return "Unknown";
  }
}

std::string SeverityToString(GLenum severity) {
  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH_ARB:
      return "high";
      break;
    case GL_DEBUG_SEVERITY_MEDIUM_ARB:
      return "medium";
      break;
    case GL_DEBUG_SEVERITY_LOW_ARB:
      return "low";
      break;
    default:
      return "unknown";
  }
}

}  // namespace

void APIENTRY OnOpenglError(GLenum source, GLenum type, GLuint id,
                              GLenum severity, GLsizei length,
                              const GLchar *message, const GLvoid *userParam) {
  // ignore non-significant error/warning codes
  if (type == GL_DEBUG_TYPE_OTHER_ARB) return;

  // only display the first 10
  static int count = 0;
  if (count > 10) return;
  ++count;

  std::cout << "---------------" << std::endl;
  std::cout << "Debug message (" << id << "): " << message << std::endl;
  std::cout << "Source " << SourceToString(source)
            << " type: " << TypeToString(type)
            << " Severity: " << SeverityToString(severity) << "\n\n";
  Assert(false);
}

void SetupOpenglDebug() {
  GLint flags;
  if (GLAD_GL_ARB_debug_output) {
    std::cout << "Enabling OpenGL debug output\n";
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
    glDebugMessageCallbackARB(OnOpenglError, nullptr);
    glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                             GL_TRUE);
  }
}