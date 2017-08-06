#include "render/materialshader.h"

#include "render/shaderattribute3d.h"
#include "core/proto.h"

#include "core/texturetypes.h"

#include "materialshader.pb.h"

MaterialShaderBinding::MaterialShaderBinding(const ShaderUniform &uniform, const EnumValue &texture_name)
  : uniform_(uniform)
  , texture_name_(texture_name)
{}

MaterialShader::MaterialShader()
  : projection_(ShaderUniform::Null())
  , view_(ShaderUniform::Null())
  , model_(ShaderUniform::Null())
{
}

bool MaterialShader::Load(FileSystem* file_system, const std::string& path) {
  attributes3d::PrebindShader(&shader_);
  const bool shader_compile = shader_.Load(file_system, path);
  if (!shader_compile) { return false; }

  materialshader::MaterialShader material_shader_file;
  const std::string proto_path = path + ".json";
  std::string error = LoadProtoJson(file_system, &material_shader_file, proto_path);
  if (!error.empty()) {
    std::cerr << "Failed to load material shader json " << path << ": " << error << "\n";
    // todo: set default shader names
  }

  for (const auto& texture : material_shader_file.textures())
  {
    const auto uniform = shader_.GetUniform(texture.uniform());
    DEFINE_ENUM_VALUE(TextureType, texture_name, texture.texture());
    std::cout << "Defining shader " << path << ": "<< texture.uniform() << " to " << texture.texture() << "\n";
    bindings_.push_back( MaterialShaderBinding{uniform, texture_name} );
  }

  // todo: get the shader names from a trusted source
  projection_ = shader_.GetUniform("uProjection");
  view_ = shader_.GetUniform("uView");
  model_ = shader_.GetUniform("uModel");

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
