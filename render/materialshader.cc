#include "render/materialshader.h"

#include <utility>

#include "core/proto.h"
#include "core/log.h"
#include "core/texturetypes.h"

#include "render/shaderattribute3d.h"
#include "render/light.h"

#include "gaf_materialshader.h"

namespace euphoria::render
{
    LOG_SPECIFY_DEFAULT_LOGGER("render.materialshader")

    ////////////////////////////////////////////////////////////////////////////////

    MaterialShaderDefaultTexture::MaterialShaderDefaultTexture(
            const core::EnumValue& a_name,
            const std::string&     a_path)
        : name(a_name), path(a_path)
    {}

    ////////////////////////////////////////////////////////////////////////////////

    MaterialShaderBinding::MaterialShaderBinding(
            ShaderUniform          a_uniform,
            const core::EnumValue& a_name)
        : uniform(std::move(a_uniform)), name(a_name)
    {}


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
    {}

    void
    PostBuild(
            MaterialShader*                       sh,
            const materialshader::MaterialShader& file,
            const std::string&                    path)
    {
        sh->hasLight_ = file.has_light;

        for(const auto& texture: file.textures)
        {
            const auto uniform = sh->shader_.GetUniform(texture.uniform);
            DEFINE_ENUM_VALUE(core::TextureType, texture_name, texture.texture);
            LOG_INFO(
                    "Defining shader " << path << ": " << texture.uniform
                                       << " to " << texture.texture);
            sh->bindings.emplace_back(uniform, texture_name);
        }

        for(const auto& texture: file.default_textures)
        {
            DEFINE_ENUM_VALUE(core::TextureType, texture_name, texture.texture);
            sh->default_textures.emplace_back(texture_name, texture.path);
        }

        // todo: get the shader names from a trusted source
        sh->projection_ = sh->shader_.GetUniform("uProjection");
        sh->view_       = sh->shader_.GetUniform("uView");
        sh->model_      = sh->shader_.GetUniform("uModel");

        if(!file.ambient.empty())
        {
            sh->ambient_ = sh->shader_.GetUniform(file.ambient);
        }
        if(!file.diffuse.empty())
        {
            sh->diffuse_ = sh->shader_.GetUniform(file.diffuse);
        }
        if(!file.specular.empty())
        {
            sh->specular_
                    = sh->shader_.GetUniform(file.specular);
        }
        if(!file.shininess.empty())
        {
            sh->shininess_
                    = sh->shader_.GetUniform(file.shininess);
        }

        if(sh->hasLight_)
        {
            sh->lightAmbient_   = sh->shader_.GetUniform("uLight.ambient");
            sh->lightDiffuse_   = sh->shader_.GetUniform("uLight.diffuse");
            sh->lightSpecular_  = sh->shader_.GetUniform("uLight.specular");
            sh->lightPosition_  = sh->shader_.GetUniform("uLight.position");
            sh->lightDirection_ = sh->shader_.GetUniform("uLight.direction");
            sh->lightType_      = sh->shader_.GetUniform("uLight.type");
            sh->lightCutoffAngleOuter_
                    = sh->shader_.GetUniform("uLight.cosCutoffAngleOuter");
            sh->lightCutoffAngleInner_
                    = sh->shader_.GetUniform("uLight.cosCutoffAngleInner");

            sh->lightAttenuationConstant_
                    = sh->shader_.GetUniform("uLight.attConst");
            sh->lightAttenuationLinear_
                    = sh->shader_.GetUniform("uLight.attLin");
            sh->lightAttenuationQuadratic_
                    = sh->shader_.GetUniform("uLight.attQuad");

            sh->normalMatrix_ = sh->shader_.GetUniform("uNormalMatrix");
            sh->viewPosition_ = sh->shader_.GetUniform("uViewPosition");
        }
    }

    bool
    MaterialShader::Load(
            core::vfs::FileSystem* file_system,
            const std::string&     path)
    {
        attributes3d::PrebindShader(&shader_);
        const bool shader_compile = shader_.Load(file_system, path);
        // if (!shader_compile) { return false; }

        materialshader::MaterialShader file;
        const std::string              proto_path = path + ".json";
        std::string                    error      = core::LoadProtoJson(
                file_system, &file, proto_path);
        if(!error.empty())
        {
            LOG_ERROR(
                    "Failed to load material shader json " << path << ": "
                                                           << error);
            // todo: set default shader names
        }

        PostBuild(this, file, path);

        return shader_compile;
    }

    void
    MaterialShader::UseShader()
    {
        Use(&shader_);
    }

    void
    MaterialShader::SetProjection(const core::mat4f& projection)
    {
        shader_.SetUniform(projection_, projection);
    }

    void
    MaterialShader::SetView(const core::mat4f& view)
    {
        shader_.SetUniform(view_, view);
    }

    void
    MaterialShader::SetModel(const core::mat4f& model)
    {
        shader_.SetUniform(model_, model);
        if(hasLight_)
        {
            core::mat4f normal   = model;
            const bool  inverted = normal.Invert();
            ASSERT(inverted);
            normal = normal.GetTransposed();
            shader_.SetUniform(normalMatrix_, normal.GetMat3());
        }
    }

    void
    MaterialShader::SetupLight(const Light& light, const core::vec3f& camera)
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
        shader_.SetUniform(
                lightCutoffAngleOuter_, Cos(light.GetCutoffAngleOuter()));
        shader_.SetUniform(
                lightCutoffAngleInner_, Cos(light.GetCutoffAngleInner()));

        const auto att = light.GetAttenuation();
        shader_.SetUniform(lightAttenuationConstant_, att.GetConstant());
        shader_.SetUniform(lightAttenuationLinear_, att.GetLinear());
        shader_.SetUniform(lightAttenuationQuadratic_, att.GetQuadratic());

        shader_.SetUniform(lightType_, static_cast<int>(light.GetType()));
        shader_.SetUniform(viewPosition_, camera);
    }

    void
    MaterialShader::SetColors(
            const core::Rgb& ambient,
            const core::Rgb& diffuse,
            const core::Rgb& specular,
            float            shininess)
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
            const auto the_specular
                    = shininess > 0 ? specular : core::Rgb {core::Color::Black};
            shader_.SetUniform(specular_, the_specular);
        }

        if(!shininess_.IsNull())
        {
            const auto the_shininess = shininess > 0 ? shininess : 1.0f;
            shader_.SetUniform(shininess_, the_shininess);
        }
    }

}  // namespace euphoria::render
