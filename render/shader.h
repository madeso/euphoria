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
#include "render/shaderuniform.h"

class ShaderId : Noncopyable {
 public:
  ShaderId();
  ~ShaderId();

  bool IsCurrentlyBound() const;

  gluint id() const;

 private:
  gluint id_;
};

void Use(const Shader *id);

class Shader : public ShaderId {
 public:
  Shader();

  // shader attribute =
  void PreBind(const ShaderAttribute& attribute);
  bool Load(const std::string& file_path);

  // todo: make private when compiled assets are available
  bool Compile(const glchar *vertexSource, const glchar *fragmentSource,
               const glchar *geometrySource = nullptr);

 public:
  // uniform = shader global
  ShaderUniform GetUniform(const std::string& name);

  void SetUniform(const ShaderUniform& attribute, glint val);
  void SetUniform(const ShaderUniform& attribute, const Rgb& val);
  void SetUniform(const ShaderUniform& attribute, const Rgba& val);
  void SetUniform(const ShaderUniform& attribute, const vec4f& val);
  void SetUniform(const ShaderUniform& attribute, const mat4f& val);

  // debug
  static const Shader* CurrentlyBound();
  const std::vector<ShaderAttribute>& GetAttributes() const;
  const std::string& GetName() const;

 private:
  // debug
  std::vector<ShaderAttribute> bound_attributes_;
  bool HasBoundAttribute(const ShaderAttribute& attribute) const;
  std::vector<ShaderUniform> bound_uniforms_;
  bool HasBoundUniform(const ShaderUniform& uniform) const;
  std::string shader_name_;
};

#endif
