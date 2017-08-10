#include "render/materialshader.h"

#include "core/proto.h"
#include "core/log.h"
#include "core/texturetypes.h"

#include "render/shaderattribute3d.h"
#include "render/light.h"

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
  , has_color_(false)
  , ambient_(ShaderUniform::Null())
  , diffuse_(ShaderUniform::Null())
  , specular_(ShaderUniform::Null())
  , shininess_(ShaderUniform::Null())
  , hasLight_(false)
  , lightAmbient_(ShaderUniform::Null())
  , lightDiffuse_(ShaderUniform::Null())
  , lightSpecular_(ShaderUniform::Null())
  , lightPosition_(ShaderUniform::Null())
  , normalMatrix_(ShaderUniform::Null())
  , viewPosition_(ShaderUniform::Null())
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
  has_color_ = material_shader_file.has_color();

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

  if(has_color_) {
    ambient_ = shader_.GetUniform("uMaterial.ambient");
    diffuse_ = shader_.GetUniform("uMaterial.diffuse");
    specular_ = shader_.GetUniform("uMaterial.specular");
    shininess_ = shader_.GetUniform("uMaterial.shininess");
  }

  if(hasLight_)
  {
    lightAmbient_ = shader_.GetUniform("uLight.ambient");
    lightDiffuse_ = shader_.GetUniform("uLight.diffuse");
    lightSpecular_ = shader_.GetUniform("uLight.specular");
    lightPosition_ = shader_.GetUniform("uLight.position");

    normalMatrix_ = shader_.GetUniform("uNormalMatrix");
    viewPosition_ = shader_.GetUniform("uViewPosition");
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
    shader_.SetUniform(normalMatrix_, normal.GetMat3());
  }
}

void MaterialShader::SetupLight(const Light& light, const vec3f& camera)
{
  if(!hasLight_) {
    return;
  }

  // todo: get light from the actual light and not a hardcoded constant
  shader_.SetUniform(lightAmbient_, Rgb{0.1f});
  shader_.SetUniform(lightDiffuse_, Rgb::From(Color::White));
  shader_.SetUniform(lightSpecular_, Rgb::From(Color::White));
  shader_.SetUniform(lightPosition_, light.GetPosition());
  shader_.SetUniform(viewPosition_, camera);
}

void MaterialShader::SetColors(const Rgb &ambient, const Rgb &diffuse, const Rgb &specular, float shininess)
{
  if(has_color_)
  {
    shader_.SetUniform(ambient_, ambient);
    shader_.SetUniform(diffuse_, diffuse);
    if(shininess > 0)
    {
      shader_.SetUniform(specular_, specular);
      shader_.SetUniform(shininess_, shininess);
    }
    else
    {
      shader_.SetUniform(specular_, Rgb::From(Color::Black));
      shader_.SetUniform(shininess_, 1.0f);
    }
  }
}

const std::vector<MaterialShaderBinding>& MaterialShader::GetBindings() const
{
  return bindings_;
}
