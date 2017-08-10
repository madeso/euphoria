#ifndef EUPHORIA_MATERIALSHADER_H
#define EUPHORIA_MATERIALSHADER_H

#include <vector>

#include "core/enum.h"
#include "render/shader.h"

class Light;

class MaterialShaderBinding
{
public:
  MaterialShaderBinding(const ShaderUniform& uniform, const EnumValue& name);

  const ShaderUniform& GetUniform() const;
  const EnumValue& GetName() const;

private:
  ShaderUniform uniform_;
  EnumValue name_;
};

class MaterialShader
{
 public:
  MaterialShader();

  bool Load(FileSystem* file_system, const std::string& path);

  void UseShader();

  void SetProjection(const mat4f& projection);
  void SetView(const mat4f& view);
  void SetModel(const mat4f& model);

  void SetupLight(const Light& light, const vec3f& camera);
  void SetColors(const Rgb& ambient, const Rgb& diffuse, const Rgb& specular);

  const std::vector<MaterialShaderBinding>& GetBindings() const;

  Shader shader_;
private:
  ShaderUniform projection_;
  ShaderUniform view_;
  ShaderUniform model_;

  bool has_color_;
  ShaderUniform ambient_;
  ShaderUniform diffuse_;
  ShaderUniform specular_;

  bool hasLight_;
  ShaderUniform ambientLight_;
  ShaderUniform lightColor_;
  ShaderUniform lightPosition_;
  ShaderUniform normalMatrix_;
  ShaderUniform viewPosition_;

  std::vector<MaterialShaderBinding> bindings_;
};


#endif //EUPHORIA_MATERIALSHADER_H
