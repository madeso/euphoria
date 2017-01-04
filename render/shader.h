#ifndef SHADER_H
#define SHADER_H

#include <string>

#include "core/vec2.h"
#include "core/vec3.h"
#include "core/vec4.h"
#include "core/mat4.h"

#include "render/gltypes.h"
#include "core/noncopyable.h"

class ShaderId : Noncopyable {
 public:
  ShaderId();
  ~ShaderId();

  bool IsCurrentlyBound() const;

  gluint id() const;

 private:
  gluint id_;
};

void Use(const ShaderId *id);

class Shader : public ShaderId {
 public:
  Shader();
  Shader(const std::string& file_path);

  void Compile(const glchar *vertexSource, const glchar *fragmentSource,
               const glchar *geometrySource = nullptr);

  void SetFloat(const glchar *name, glfloat value);
  void SetInteger(const glchar *name, glint value);
  void SetVector2f(const glchar *name, glfloat x, glfloat y);
  void SetVector2f(const glchar *name, const vec2f &value);
  void SetVector3f(const glchar *name, glfloat x, glfloat y, glfloat z);
  void SetVector3f(const glchar *name, const vec3f &value);
  void SetVector4f(const glchar *name, glfloat x, glfloat y, glfloat z,
                   glfloat w);
  void SetVector4f(const glchar *name, const vec4f &value);
  void SetMatrix4(const glchar *name, const mat4f &matrix);
};

#endif
