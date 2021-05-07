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

    MaterialShaderDefaultTexture::MaterialShaderDefaultTexture
    (
        const core::enum_value& a_name,
        const core::vfs::FilePath& a_path
    )
        : name(a_name)
        , path(a_path)
    {}

    ////////////////////////////////////////////////////////////////////////////////

    MaterialShaderBinding::MaterialShaderBinding
    (
        ShaderUniform a_uniform,
        const core::enum_value& a_name
    )
        : uniform(std::move(a_uniform))
        , name(a_name)
    {}


    ////////////////////////////////////////////////////////////////////////////////

    MaterialShader::MaterialShader()
        : projection(ShaderUniform::Null())
        , view(ShaderUniform::Null())
        , model(ShaderUniform::Null())
        , ambient(ShaderUniform::Null())
        , diffuse(ShaderUniform::Null())
        , specular(ShaderUniform::Null())
        , shininess(ShaderUniform::Null())
        , hasLight(false)
        , lightAmbient(ShaderUniform::Null())
        , lightDiffuse(ShaderUniform::Null())
        , lightSpecular(ShaderUniform::Null())
        , lightPosition(ShaderUniform::Null())
        , lightDirection(ShaderUniform::Null())
        , lightType(ShaderUniform::Null())
        , lightAttenuationConstant(ShaderUniform::Null())
        , lightAttenuationLinear(ShaderUniform::Null())
        , lightAttenuationQuadratic(ShaderUniform::Null())
        , lightCutoffAngleOuter(ShaderUniform::Null())
        , lightCutoffAngleInner(ShaderUniform::Null())
        , normalMatrix(ShaderUniform::Null())
        , viewPosition(ShaderUniform::Null())
    {}

    void
    PostBuild
    (
        MaterialShader* sh,
        const materialshader::MaterialShader& file,
        const core::vfs::FilePath& path
    )
    {
        sh->hasLight = file.has_light;

        for(const auto& texture: file.textures)
        {
            const auto uniform = sh->shader.GetUniform(texture.uniform);
            DEFINE_ENUM_VALUE(core::TextureType, texture_name, texture.texture);
            LOG_INFO
            (
                "Defining shader {0}: {1} to {2}",
                path,
                texture.uniform,
                texture.texture
            );
            sh->bindings.emplace_back(uniform, texture_name);
        }

        for(const auto& texture: file.default_textures)
        {
            const auto texture_path = core::vfs::FilePath::FromScript(texture.path);
            if(texture_path.has_value() == false)
            {
                LOG_WARN
                (
                    "Ignored invalid path for {0} in {1}: {2}",
                    texture.texture,
                    path,
                    texture.path
                );
                continue;
            }
            DEFINE_ENUM_VALUE(core::TextureType, texture_name, texture.texture);
            sh->default_textures.emplace_back
            (
                texture_name,
                texture_path.value()
            );
        }

        // todo(Gustav): get the shader names from a trusted source
        sh->projection = sh->shader.GetUniform("uProjection");
        sh->view       = sh->shader.GetUniform("uView");
        sh->model      = sh->shader.GetUniform("uModel");

        if(!file.ambient.empty())
        {
            sh->ambient = sh->shader.GetUniform(file.ambient);
        }
        if(!file.diffuse.empty())
        {
            sh->diffuse = sh->shader.GetUniform(file.diffuse);
        }
        if(!file.specular.empty())
        {
            sh->specular = sh->shader.GetUniform(file.specular);
        }
        if(!file.shininess.empty())
        {
            sh->shininess = sh->shader.GetUniform(file.shininess);
        }

        if(sh->hasLight)
        {
            sh->lightAmbient   = sh->shader.GetUniform("uLight.ambient");
            sh->lightDiffuse   = sh->shader.GetUniform("uLight.diffuse");
            sh->lightSpecular  = sh->shader.GetUniform("uLight.specular");
            sh->lightPosition  = sh->shader.GetUniform("uLight.position");
            sh->lightDirection = sh->shader.GetUniform("uLight.direction");
            sh->lightType      = sh->shader.GetUniform("uLight.type");
            sh->lightCutoffAngleOuter
                    = sh->shader.GetUniform("uLight.cosCutoffAngleOuter");
            sh->lightCutoffAngleInner
                    = sh->shader.GetUniform("uLight.cosCutoffAngleInner");

            sh->lightAttenuationConstant
                    = sh->shader.GetUniform("uLight.attConst");
            sh->lightAttenuationLinear = sh->shader.GetUniform("uLight.attLin");
            sh->lightAttenuationQuadratic
                    = sh->shader.GetUniform("uLight.attQuad");

            sh->normalMatrix = sh->shader.GetUniform("uNormalMatrix");
            sh->viewPosition = sh->shader.GetUniform("uViewPosition");
        }
    }

    bool
    MaterialShader::Load
    (
        core::vfs::FileSystem* file_system,
        const core::vfs::FilePath& path
    )
    {
        attributes3d::PrebindShader(&shader);
        const bool shader_compile = shader.Load(file_system, path);
        // if (!shader_compile) { return false; }

        materialshader::MaterialShader file;
        const auto proto_path = path.SetExtensionCopy("json");
        std::string error = core::read_json_to_gaf_struct_or_get_error_message
        (
            file_system,
            &file,
            proto_path
        );
        if(!error.empty())
        {
            LOG_ERROR
            (
                "Failed to load material shader json {0}: {1}",
                path,
                error
            );
            // todo(Gustav): set default shader names
        }

        PostBuild(this, file, path);

        return shader_compile;
    }

    void
    MaterialShader::UseShader()
    {
        Use(&shader);
    }

    void
    MaterialShader::SetProjection(const core::mat4f& projection_data)
    {
        shader.SetUniform(projection, projection_data);
    }

    void
    MaterialShader::SetView(const core::mat4f& view_data)
    {
        shader.SetUniform(view, view_data);
    }

    void
    MaterialShader::SetModel(const core::mat4f& model_data)
    {
        shader.SetUniform(model, model_data);
        if(hasLight)
        {
            core::mat4f normal   = model_data;
            const bool  inverted = normal.invert();
            ASSERT(inverted);
            normal = normal.get_transposed();
            shader.SetUniform(normalMatrix, normal.get_mat3());
        }
    }

    void
    MaterialShader::SetupLight(const Light& light, const core::vec3f& camera)
    {
        if(!hasLight)
        {
            return;
        }

        shader.SetUniform(lightAmbient, light.ambient);
        shader.SetUniform(lightDiffuse, light.diffuse);
        shader.SetUniform(lightSpecular, light.specular);
        shader.SetUniform(lightPosition, light.position);
        shader.SetUniform(lightDirection, light.direction);
        shader.SetUniform
        (
            lightCutoffAngleOuter,
            cos(light.cutoff_angle_outer)
        );
        shader.SetUniform
        (
            lightCutoffAngleInner,
            cos(light.cutoff_angle_inner)
        );

        const auto att = light.attenuation;
        shader.SetUniform(lightAttenuationConstant, att.GetConstant());
        shader.SetUniform(lightAttenuationLinear, att.GetLinear());
        shader.SetUniform(lightAttenuationQuadratic, att.GetQuadratic());

        shader.SetUniform(lightType, static_cast<int>(light.type));
        shader.SetUniform(viewPosition, camera);
    }

    void
    MaterialShader::SetColors(
            const core::rgb& ambient_data,
            const core::rgb& diffuse_data,
            const core::rgb& specular_data,
            float            shininess_data)
    {
        if(!ambient.IsNull())
        {
            shader.SetUniform(ambient, ambient_data);
        }

        if(!diffuse.IsNull())
        {
            shader.SetUniform(diffuse, diffuse_data);
        }

        if(!specular.IsNull())
        {
            const auto the_specular = shininess_data > 0
                                              ? specular_data
                                              : core::rgb {core::color::black};
            shader.SetUniform(specular, the_specular);
        }

        if(!shininess.IsNull())
        {
            // todo(Gustav): change to 0 instead of 1.0
            const auto the_shininess
                    = shininess_data > 0 ? shininess_data : 1.0f;
            shader.SetUniform(shininess, the_shininess);
        }
    }

}  // namespace euphoria::render
