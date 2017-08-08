#include "render/materialshader.h"

#include "render/shaderattribute3d.h"
#include "core/proto.h"
#include "core/log.h"

#include "core/texturetypes.h"

#include "materialshader.pb.h"

LOG_SPECIFY_DEFAULT_LOGGER("render.materialshader")

MaterialShaderBinding::MaterialShaderBinding(const ShaderUniform& uniform, const EnumValue& name)
  : uniform_(uniform)
  , name_(name)
{}

const ShaderUniform& MaterialShaderBinding::GetUniform() const
{
  return uniform_;
}

const EnumValue& MaterialShaderBinding::GetName() const
{
  return name_;
}

MaterialShader::MaterialShader()
  : projection_(ShaderUniform::Null())
  , view_(ShaderUniform::Null())
  , model_(ShaderUniform::Null())
  , hasLight_(false)
  , ambientLight_(ShaderUniform::Null())
  , lightColor_(ShaderUniform::Null())
  , lightPosition_(ShaderUniform::Null())
  , normalMatrix_(ShaderUniform::Null())
{
}

bool MaterialShader::Load(FileSystem* file_system, const std::string& path) {
  attributes3d::PrebindShader(&shader_);
  const bool shader_compile = shader_.Load(file_system, path);

  // if (!shader_compile) { return false; }

  materialshader::MaterialShader material_shader_file;
  const std::string proto_path = path + ".json";
  std::string error = LoadProtoJson(file_system, &material_shader_file, proto_path);
  if (!error.empty()) {
    std::cerr << "Failed to load material shader json " << path << ": " << error << "\n";
    // todo: set default shader names
  }

  hasLight_ = material_shader_file.has_light();

  for (const auto& texture : material_shader_file.textures())
  {
    const auto uniform = shader_.GetUniform(texture.uniform());
    DEFINE_ENUM_VALUE(TextureType, texture_name, texture.texture());
    LOG_INFO( "Defining shader " << path << ": "<< texture.uniform() << " to " << texture.texture() );
    bindings_.push_back( MaterialShaderBinding{uniform, texture_name} );
  }

  // todo: get the shader names from a trusted source
  projection_ = shader_.GetUniform("uProjection");
  view_ = shader_.GetUniform("uView");
  model_ = shader_.GetUniform("uModel");

  if(hasLight_)
  {
    ambientLight_ = shader_.GetUniform("uAmbient");
    lightColor_ = shader_.GetUniform("uLightColor");
    lightPosition_ = shader_.GetUniform("uLightPosition");
    normalMatrix_ = shader_.GetUniform("uNormalMatrix");
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
  if(hasLight_) {
    mat4f normal = model;
    const bool inverted = normal.Invert();
    Assert(inverted);
    normal = normal.GetTransposed();
    shader_.SetUniform(normalMatrix_, normal);
  }
}

void MaterialShader::SetupLight()
{
  if(!hasLight_) {
    return;
  }

  // todo: get light from the actual light and not a hardcoded constant
  shader_.SetUniform(ambientLight_, 0.1f);
  shader_.SetUniform(lightColor_, Rgb::From(Color::White));
  shader_.SetUniform(lightPosition_, vec3f(0,0,0));
}

const std::vector<MaterialShaderBinding>& MaterialShader::GetBindings() const
{
  return bindings_;
}
