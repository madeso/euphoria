#include "render/materialshader.h"

#include <utility>

#include "core/proto.h"
#include "core/log.h"
#include "core/texturetypes.h"

#include "render/shaderattribute3d.h"
#include "render/light.h"

#include "materialshader.pb.h"

LOG_SPECIFY_DEFAULT_LOGGER("render.materialshader")

MaterialShaderBinding::MaterialShaderBinding(ShaderUniform    uniform,
                                             const EnumValue& name)
    : uniform_(std::move(uniform))
    , name_(name)
{
}

const ShaderUniform&
MaterialShaderBinding::GetUniform() const
{
  return uniform_;
}

const EnumValue&
MaterialShaderBinding::GetName() const
{
  return name_;
}

MaterialShader::MaterialShader()
    : projection_(ShaderUniform::Null())
    , view_(ShaderUniform::Null())
    , model_(ShaderUniform::Null())
    , ambient_(ShaderUniform::Null())
    , diffuse_(ShaderUniform::Null())
    , specular_(ShaderUniform::Null())
    , shininess_(ShaderUniform::Null())
    , hasLight_(false)
    , lightAmbient_(ShaderUniform::Null())
    , lightDiffuse_(ShaderUniform::Null())
    , lightSpecular_(ShaderUniform::Null())
    , lightPosition_(ShaderUniform::Null())
    , lightDirection_(ShaderUniform::Null())
    , lightType_(ShaderUniform::Null())
    , lightAttenuationConstant_(ShaderUniform::Null())
    , lightAttenuationLinear_(ShaderUniform::Null())
    , lightAttenuationQuadratic_(ShaderUniform::Null())
    , lightCutoffAngle_(ShaderUniform::Null())
    , normalMatrix_(ShaderUniform::Null())
    , viewPosition_(ShaderUniform::Null())
{
}

bool
MaterialShader::Load(FileSystem* file_system, const std::string& path)
{
  attributes3d::PrebindShader(&shader_);
  const bool shader_compile = shader_.Load(file_system, path);

  // if (!shader_compile) { return false; }

  materialshader::MaterialShader material_shader_file;
  const std::string              proto_path = path + ".json";
  std::string                    error =
      LoadProtoJson(file_system, &material_shader_file, proto_path);
  if(!error.empty())
  {
    std::cerr << "Failed to load material shader json " << path << ": " << error
              << "\n";
    // todo: set default shader names
  }

  hasLight_ = material_shader_file.has_light();

  for(const auto& texture : material_shader_file.textures())
  {
    const auto uniform = shader_.GetUniform(texture.uniform());
    DEFINE_ENUM_VALUE(TextureType, texture_name, texture.texture());
    LOG_INFO("Defining shader " << path << ": " << texture.uniform() << " to "
                                << texture.texture());
    bindings_.emplace_back(uniform, texture_name);
  }

  // todo: get the shader names from a trusted source
  projection_ = shader_.GetUniform("uProjection");
  view_       = shader_.GetUniform("uView");
  model_      = shader_.GetUniform("uModel");

  if(material_shader_file.has_ambient())
  {
    ambient_ = shader_.GetUniform(material_shader_file.ambient());
  }
  if(material_shader_file.has_diffuse())
  {
    diffuse_ = shader_.GetUniform(material_shader_file.diffuse());
  }
  if(material_shader_file.has_specular())
  {
    specular_ = shader_.GetUniform(material_shader_file.specular());
  }
  if(material_shader_file.has_shininess())
  {
    shininess_ = shader_.GetUniform(material_shader_file.shininess());
  }

  if(hasLight_)
  {
    lightAmbient_     = shader_.GetUniform("uLight.ambient");
    lightDiffuse_     = shader_.GetUniform("uLight.diffuse");
    lightSpecular_    = shader_.GetUniform("uLight.specular");
    lightPosition_    = shader_.GetUniform("uLight.position");
    lightDirection_   = shader_.GetUniform("uLight.direction");
    lightType_        = shader_.GetUniform("uLight.type");
    lightCutoffAngle_ = shader_.GetUniform("uLight.cosCutoffAngle");

    lightAttenuationConstant_  = shader_.GetUniform("uLight.attConst");
    lightAttenuationLinear_    = shader_.GetUniform("uLight.attLin");
    lightAttenuationQuadratic_ = shader_.GetUniform("uLight.attQuad");

    normalMatrix_ = shader_.GetUniform("uNormalMatrix");
    viewPosition_ = shader_.GetUniform("uViewPosition");
  }

  return shader_compile;
}

void
MaterialShader::UseShader()
{
  Use(&shader_);
}

void
MaterialShader::SetProjection(const mat4f& projection)
{
  shader_.SetUniform(projection_, projection);
}

void
MaterialShader::SetView(const mat4f& view)
{
  shader_.SetUniform(view_, view);
}

void
MaterialShader::SetModel(const mat4f& model)
{
  shader_.SetUniform(model_, model);
  if(hasLight_)
  {
    mat4f      normal   = model;
    const bool inverted = normal.Invert();
    ASSERT(inverted);
    normal = normal.GetTransposed();
    shader_.SetUniform(normalMatrix_, normal.GetMat3());
  }
}

void
MaterialShader::SetupLight(const Light& light, const vec3f& camera)
{
  if(!hasLight_)
  {
    return;
  }

  shader_.SetUniform(lightAmbient_, light.GetAmbient());
  shader_.SetUniform(lightDiffuse_, light.GetDiffuse());
  shader_.SetUniform(lightSpecular_, light.GetSpecular());
  shader_.SetUniform(lightPosition_, light.GetPosition());
  shader_.SetUniform(lightDirection_, light.GetDirection());
  shader_.SetUniform(lightCutoffAngle_, Cos(light.GetCutoffAngle()));

  const auto att = light.GetAttenuation();
  shader_.SetUniform(lightAttenuationConstant_, att.GetConstant());
  shader_.SetUniform(lightAttenuationLinear_, att.GetLinear());
  shader_.SetUniform(lightAttenuationQuadratic_, att.GetQuadratic());

  shader_.SetUniform(lightType_, static_cast<int>(light.GetType()));
  shader_.SetUniform(viewPosition_, camera);
}

void
MaterialShader::SetColors(const Rgb& ambient, const Rgb& diffuse,
                          const Rgb& specular, float shininess)
{
  if(!ambient_.IsNull())
  {
    shader_.SetUniform(ambient_, ambient);
  }

  if(!diffuse_.IsNull())
  {
    shader_.SetUniform(diffuse_, diffuse);
  }

  if(!specular_.IsNull())
  {
    const auto the_specular =
        shininess > 0 ? specular : Rgb::From(Color::Black);
    shader_.SetUniform(specular_, the_specular);
  }

  if(!shininess_.IsNull())
  {
    const auto the_shininess = shininess > 0 ? shininess : 1.0f;
    shader_.SetUniform(shininess_, the_shininess);
  }
}

const std::vector<MaterialShaderBinding>&
MaterialShader::GetBindings() const
{
  return bindings_;
}
