#ifndef SHADER_H
#define SHADER_H

#include <string>

#include "core/vec2.h"
#include "core/vec3.h"
#include "core/vec4.h"
#include "core/mat4.h"

#include "spacetyper/gl.h"
#include "core/noncopyable.h"

class ShaderId : Noncopyable {
 public:
  ShaderId();
  ~ShaderId();

  bool IsCurrentlyBound() const;

  GLuint id() const;

 private:
  GLuint id_;
};

void Use(const ShaderId *id);

class Shader : public ShaderId {
 public:
  Shader();
  Shader(const std::string& file_path);

  void Compile(const GLchar *vertexSource, const GLchar *fragmentSource,
               const GLchar *geometrySource = nullptr);

  void SetFloat(const GLchar *name, GLfloat value);
  void SetInteger(const GLchar *name, GLint value);
  void SetVector2f(const GLchar *name, GLfloat x, GLfloat y);
  void SetVector2f(const GLchar *name, const vec2f &value);
  void SetVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z);
  void SetVector3f(const GLchar *name, const vec3f &value);
  void SetVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z,
                   GLfloat w);
  void SetVector4f(const GLchar *name, const vec4f &value);
  void SetMatrix4(const GLchar *name, const mat4f &matrix);
};

#endif
