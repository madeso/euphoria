#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <vector>

#include "core/vec2.h"
#include "core/vec3.h"
#include "core/vec4.h"
#include "core/rgb.h"
#include "core/mat4.h"

#include "render/gltypes.h"
#include "core/noncopyable.h"

#include "render/shaderattribute.h"

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

  void PreBind(const ShaderAttribute& attribute);
  bool Load(const std::string& file_path);

  void Compile(const glchar *vertexSource, const glchar *fragmentSource,
               const glchar *geometrySource = nullptr);

  void SetUniform(const ShaderAttribute& attribute, glint val);
  void SetUniform(const ShaderAttribute& attribute, const Rgb& val);
  void SetUniform(const ShaderAttribute& attribute, const Rgba& val);
  void SetUniform(const ShaderAttribute& attribute, const vec4f& val);
  void SetUniform(const ShaderAttribute& attribute, const mat4f& val);

 private:
  // debug
  std::vector<ShaderAttribute> bound_attributes_;
  bool HasBoundAttribute(const ShaderAttribute& attribute) const;
};

#endif
