#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "spacetyper/gl.h"

class ShaderId {
 public:
  ShaderId();
  ~ShaderId();

  bool IsCurrentlyBound() const;

  GLuint id() const;

 private:
  GLuint id_;
};

void Use(ShaderId *id);

class Shader : public ShaderId {
 public:
  Shader();
  Shader(const GLchar *vertexSource, const GLchar *fragmentSource,
         const GLchar *geometrySource = nullptr);

  void Compile(const GLchar *vertexSource, const GLchar *fragmentSource,
               const GLchar *geometrySource = nullptr);

  void SetFloat(const GLchar *name, GLfloat value);
  void SetInteger(const GLchar *name, GLint value);
  void SetVector2f(const GLchar *name, GLfloat x, GLfloat y);
  void SetVector2f(const GLchar *name, const glm::vec2 &value);
  void SetVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z);
  void SetVector3f(const GLchar *name, const glm::vec3 &value);
  void SetVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z,
                   GLfloat w);
  void SetVector4f(const GLchar *name, const glm::vec4 &value);
  void SetMatrix4(const GLchar *name, const glm::mat4 &matrix);
};

#endif
