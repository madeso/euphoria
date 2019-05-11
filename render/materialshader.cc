#include "render/materialshader.h"

#include <utility>

#include "core/proto.h"
#include "core/log.h"
#include "core/texturetypes.h"

#include "render/shaderattribute3d.h"
#include "render/light.h"

#include "gaf_materialshader.h"

LOG_SPECIFY_DEFAULT_LOGGER("render.materialshader")

////////////////////////////////////////////////////////////////////////////////

MaterialShaderDefaultTexture::MaterialShaderDefaultTexture(
    const EnumValue& name, const std::string& path)
    : name_(name)
    , path_(path)
{
}

const EnumValue&
MaterialShaderDefaultTexture::GetName() const
{
  return name_;
}

const std::string&
MaterialShaderDefaultTexture::GetPath() const
{
  return path_;
}

////////////////////////////////////////////////////////////////////////////////

MaterialShaderBinding::MaterialShaderBinding(
    ShaderUniform uniform, const EnumValue& name)
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


////////////////////////////////////////////////////////////////////////////////

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
    , lightCutoffAngleOuter_(ShaderUniform::Null())
    , lightCutoffAngleInner_(ShaderUniform::Null())
    , normalMatrix_(ShaderUniform::Null())
    , viewPosition_(ShaderUniform::Null())
{
}

void
PostBuild(
    MaterialShader*                       sh,
    const materialshader::MaterialShader& material_shader_file,
    const std::string&                    path)
{
  sh->hasLight_ = material_shader_file.has_light;

  for(const auto& texture : material_shader_file.textures)
  {
    const auto uniform = sh->shader_.GetUniform(texture.uniform);
    DEFINE_ENUM_VALUE(TextureType, texture_name, texture.texture);
    LOG_INFO(
        "Defining shader " << path << ": " << texture.uniform << " to "
                           << texture.texture);
    sh->bindings_.emplace_back(uniform, texture_name);
  }

  for(const auto& texture : material_shader_file.default_textures)
  {
    DEFINE_ENUM_VALUE(TextureType, texture_name, texture.texture);
    sh->default_textures_.emplace_back(texture_name, texture.path);
  }

  // todo: get the shader names from a trusted source
  sh->projection_ = sh->shader_.GetUniform("uProjection");
  sh->view_       = sh->shader_.GetUniform("uView");
  sh->model_      = sh->shader_.GetUniform("uModel");

  if(!material_shader_file.ambient.empty())
  {
    sh->ambient_ = sh->shader_.GetUniform(material_shader_file.ambient);
  }
  if(!material_shader_file.diffuse.empty())
  {
    sh->diffuse_ = sh->shader_.GetUniform(material_shader_file.diffuse);
  }
  if(!material_shader_file.specular.empty())
  {
    sh->specular_ = sh->shader_.GetUniform(material_shader_file.specular);
  }
  if(!material_shader_file.shininess.empty())
  {
    sh->shininess_ = sh->shader_.GetUniform(material_shader_file.shininess);
  }

  if(sh->hasLight_)
  {
    sh->lightAmbient_   = sh->shader_.GetUniform("uLight.ambient");
    sh->lightDiffuse_   = sh->shader_.GetUniform("uLight.diffuse");
    sh->lightSpecular_  = sh->shader_.GetUniform("uLight.specular");
    sh->lightPosition_  = sh->shader_.GetUniform("uLight.position");
    sh->lightDirection_ = sh->shader_.GetUniform("uLight.direction");
    sh->lightType_      = sh->shader_.GetUniform("uLight.type");
    sh->lightCutoffAngleOuter_ =
        sh->shader_.GetUniform("uLight.cosCutoffAngleOuter");
    sh->lightCutoffAngleInner_ =
        sh->shader_.GetUniform("uLight.cosCutoffAngleInner");

    sh->lightAttenuationConstant_  = sh->shader_.GetUniform("uLight.attConst");
    sh->lightAttenuationLinear_    = sh->shader_.GetUniform("uLight.attLin");
    sh->lightAttenuationQuadratic_ = sh->shader_.GetUniform("uLight.attQuad");

    sh->normalMatrix_ = sh->shader_.GetUniform("uNormalMatrix");
    sh->viewPosition_ = sh->shader_.GetUniform("uViewPosition");
  }
}

bool
MaterialShader::Load(vfs::FileSystem* file_system, const std::string& path)
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

  PostBuild(this, material_shader_file, path);

  return shader_compile;
}

bool
MaterialShader::Compile(
    const glchar* vertex, const glchar* fragment, const glchar* geom)
{
  attributes3d::PrebindShader(&shader_);
  const bool shader_compile = shader_.Compile(vertex, fragment, geom);
  // if (!shader_compile) { return false; }

  materialshader::MaterialShader material_shader_file;
  PostBuild(this, material_shader_file, "[source compile]");

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
MaterialShader::SetupLight(const Light& light, const point3f& camera)
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
  shader_.SetUniform(lightCutoffAngleOuter_, Cos(light.GetCutoffAngleOuter()));
  shader_.SetUniform(lightCutoffAngleInner_, Cos(light.GetCutoffAngleInner()));

  const auto att = light.GetAttenuation();
  shader_.SetUniform(lightAttenuationConstant_, att.GetConstant());
  shader_.SetUniform(lightAttenuationLinear_, att.GetLinear());
  shader_.SetUniform(lightAttenuationQuadratic_, att.GetQuadratic());

  shader_.SetUniform(lightType_, static_cast<int>(light.GetType()));
  shader_.SetUniform(viewPosition_, camera);
}

void
MaterialShader::SetColors(
    const Rgb& ambient,
    const Rgb& diffuse,
    const Rgb& specular,
    float      shininess)
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
    const auto the_specular = shininess > 0 ? specular : Rgb{Color::Black};
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

const std::vector<MaterialShaderDefaultTexture>&
MaterialShader::GetDefaultTextures()
{
  return default_textures_;
}
