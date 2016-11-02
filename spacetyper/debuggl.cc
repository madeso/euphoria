#include "spacetyper/debuggl.h"

#include <iostream>
#include "spacetyper/gl.h"

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
    case GL_STACK_OVERFLOW:
      return "STACK_OVERFLOW";
      break;
    case GL_STACK_UNDERFLOW:
      return "STACK_UNDERFLOW";
      break;
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
    case GL_DEBUG_SOURCE_API:
      return "API";
      break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      return "Window System";
      break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      return "Shader Compiler";
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      return "Third Party";
      break;
    case GL_DEBUG_SOURCE_APPLICATION:
      return "Application";
      break;
    case GL_DEBUG_SOURCE_OTHER:
      return "Other";
      break;
    default:
      return "Unknown";
  }
}

std::string TypeToString(GLenum type) {
  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      return "Error";
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      return "Deprecated Behaviour";
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      return "Undefined Behaviour";
      break;
    case GL_DEBUG_TYPE_PORTABILITY:
      return "Portability";
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:
      return "Performance";
      break;
    case GL_DEBUG_TYPE_MARKER:
      return "Marker";
      break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
      return "Push Group";
      break;
    case GL_DEBUG_TYPE_POP_GROUP:
      return "Pop Group";
      break;
    case GL_DEBUG_TYPE_OTHER:
      return "Other";
      break;
    default:
      return "Unknown";
  }
}

std::string SeverityToString(GLenum severity) {
  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      return "high";
      break;
    case GL_DEBUG_SEVERITY_MEDIUM:
      return "medium";
      break;
    case GL_DEBUG_SEVERITY_LOW:
      return "low";
      break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      return "notification";
      break;
    default:
      return "unknown";
  }
}

}  // namespace


void GLAPIENTRY OnOpenglError(GLenum source, GLenum type, GLuint id, GLenum severity,
                              GLsizei length, const GLchar *message,
                              const GLvoid* userParam) {
  // ignore non-significant error/warning codes
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

  static int count = 0;
  if( count > 10 ) return;
  ++count;

  std::cout << "---------------" << std::endl;
  std::cout << "Debug message (" << id << "): " << message << std::endl;
  std::cout << "Source " << SourceToString(source) << " type: " << TypeToString(type) << " Severity: " << SeverityToString(severity) << "\n\n";
}
void SetupOpenglDebug() {
  GLint flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    std::cout << "Enabling opengl output core\n";
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OnOpenglError, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                          GL_TRUE);
  }
}