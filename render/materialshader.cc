#include "render/materialshader.h"

#include "render/shaderattribute3d.h"

MaterialShader::MaterialShader()
  : projection_(ShaderUniform::Null())
  , view_(ShaderUniform::Null())
  , model_(ShaderUniform::Null())
{
}

bool MaterialShader::Load(FileSystem* file_system, const std::string& path)
{
  attributes3d::PrebindShader(&shader_);
  const bool shader_compile = shader_.Load(file_system, path);

  if(shader_compile) {
    // todo: get the shader names from a trusted source
    projection_ = shader_.GetUniform("uProjection");
    view_ = shader_.GetUniform("uView");
    model_ = shader_.GetUniform("uModel");
  }

  return shader_compile;
}

void MaterialShader::UseShader()
{
  Use(&shader_);
}

void MaterialShader::SetProjection(const mat4f& projection)
{
  shader_.SetUniform(projection_, projection);
}

void MaterialShader::SetView(const mat4f& view)
{
  shader_.SetUniform(view_, view);
}

void MaterialShader::SetModel(const mat4f& model)
{
  shader_.SetUniform(model_, model);
}
