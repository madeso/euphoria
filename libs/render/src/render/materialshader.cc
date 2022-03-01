#include "render/materialshader.h"

#include <utility>

#include "core/proto.h"
#include "log/log.h"
#include "core/texturetypes.h"

#include "render/shaderattribute3d.h"
#include "render/light.h"

#include "gaf_materialshader.h"
#include "gaf_rapidjson_materialshader.h"

namespace euphoria::render
{
    MaterialShaderDefaultTexture::MaterialShaderDefaultTexture
    (
        const core::EnumValue& a_name,
        const core::vfs::FilePath& a_path
    )
        : name(a_name)
        , path(a_path)
    {
    }

    ////////////////////////////////////////////////////////////////////////////////

    MaterialShaderBinding::MaterialShaderBinding
    (
        ShaderUniform a_uniform,
        const core::EnumValue& a_name
    )
        : uniform(std::move(a_uniform))
        , name(a_name)
    {
    }


    ////////////////////////////////////////////////////////////////////////////////

    MaterialShader::MaterialShader()
        : projection(ShaderUniform::null())
        , view(ShaderUniform::null())
        , model(ShaderUniform::null())
        , ambient(ShaderUniform::null())
        , diffuse(ShaderUniform::null())
        , specular(ShaderUniform::null())
        , shininess(ShaderUniform::null())
        , has_light(false)
        , light_ambient(ShaderUniform::null())
        , light_diffuse(ShaderUniform::null())
        , light_specular(ShaderUniform::null())
        , light_position(ShaderUniform::null())
        , light_direction(ShaderUniform::null())
        , light_type(ShaderUniform::null())
        , light_attenuation_constant(ShaderUniform::null())
        , light_attenuation_linear(ShaderUniform::null())
        , light_attenuation_quadratic(ShaderUniform::null())
        , light_cutoff_angle_outer(ShaderUniform::null())
        , light_cutoff_angle_inner(ShaderUniform::null())
        , normal_matrix(ShaderUniform::null())
        , view_position(ShaderUniform::null())
    {
    }

    void
    post_build
    (
        MaterialShader* sh,
        const materialshader::MaterialShader& file,
        const core::vfs::FilePath& path
    )
    {
        sh->has_light = file.has_light;

        for(const auto& texture: file.textures)
        {
            const auto uniform = sh->shader.get_uniform(texture.uniform);
            DEFINE_ENUM_VALUE(core::texture_type, texture_name, texture.texture);
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
            const auto texture_path = core::vfs::FilePath::from_script(texture.path);
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
            DEFINE_ENUM_VALUE(core::texture_type, texture_name, texture.texture);
            sh->default_textures.emplace_back
            (
                texture_name,
                texture_path.value()
            );
        }

        // todo(Gustav): get the shader names from a trusted source
        sh->projection = sh->shader.get_uniform("uProjection");
        sh->view = sh->shader.get_uniform("uView");
        sh->model = sh->shader.get_uniform("uModel");

        if(!file.ambient.empty())
        {
            sh->ambient = sh->shader.get_uniform(file.ambient);
        }
        if(!file.diffuse.empty())
        {
            sh->diffuse = sh->shader.get_uniform(file.diffuse);
        }
        if(!file.specular.empty())
        {
            sh->specular = sh->shader.get_uniform(file.specular);
        }
        if(!file.shininess.empty())
        {
            sh->shininess = sh->shader.get_uniform(file.shininess);
        }

        if(sh->has_light)
        {
            sh->light_ambient = sh->shader.get_uniform("uLight.ambient");
            sh->light_diffuse = sh->shader.get_uniform("uLight.diffuse");
            sh->light_specular = sh->shader.get_uniform("uLight.specular");
            sh->light_position = sh->shader.get_uniform("uLight.position");
            sh->light_direction = sh->shader.get_uniform("uLight.direction");
            sh->light_type = sh->shader.get_uniform("uLight.type");
            sh->light_cutoff_angle_outer = sh->shader.get_uniform("uLight.cosCutoffAngleOuter");
            sh->light_cutoff_angle_inner = sh->shader.get_uniform("uLight.cosCutoffAngleInner");

            sh->light_attenuation_constant = sh->shader.get_uniform("uLight.attConst");
            sh->light_attenuation_linear = sh->shader.get_uniform("uLight.attLin");
            sh->light_attenuation_quadratic = sh->shader.get_uniform("uLight.attQuad");

            sh->normal_matrix = sh->shader.get_uniform("uNormalMatrix");
            sh->view_position = sh->shader.get_uniform("uViewPosition");
        }
    }

    bool
    MaterialShader::load
    (
        core::vfs::FileSystem* file_system,
        const core::vfs::FilePath& path
    )
    {
        attributes3d::prebind_shader(&shader);
        const bool shader_compile = shader.load(file_system, path);
        // if (!shader_compile) { return false; }

        const auto proto_path = path.set_extension_copy("json");
        const auto file = core::get_default_but_log_errors
        (
            core::read_json_file_to_gaf_struct<materialshader::MaterialShader>(file_system, proto_path, materialshader::ReadJsonMaterialShader)
        );
        // todo(Gustav): set default shader names if file failed to

        post_build(this, file, path);

        return shader_compile;
    }

    void
    MaterialShader::use_shader()
    {
        use(&shader);
    }

    void
    MaterialShader::set_projection(const core::Mat4f& projection_data)
    {
        shader.set_uniform(projection, projection_data);
    }

    void
    MaterialShader::set_view(const core::Mat4f& view_data)
    {
        shader.set_uniform(view, view_data);
    }

    void
    MaterialShader::set_model(const core::Mat4f& model_data)
    {
        shader.set_uniform(model, model_data);
        if(has_light)
        {
            core::Mat4f normal = model_data;
            const bool inverted = normal.invert();
            ASSERT(inverted);
            normal = normal.get_transposed();
            shader.set_uniform(normal_matrix, normal.get_mat3());
        }
    }

    void
    MaterialShader::setup_light(const Light& light, const core::Vec3f& camera)
    {
        if(!has_light)
        {
            return;
        }

        shader.set_uniform(light_ambient, light.ambient);
        shader.set_uniform(light_diffuse, light.diffuse);
        shader.set_uniform(light_specular, light.specular);
        shader.set_uniform(light_position, light.position);
        shader.set_uniform(light_direction, light.direction);
        shader.set_uniform(light_cutoff_angle_outer, cos(light.cutoff_angle_outer));
        shader.set_uniform(light_cutoff_angle_inner, cos(light.cutoff_angle_inner));

        const auto att = light.attenuation;
        shader.set_uniform(light_attenuation_constant, att.constant);
        shader.set_uniform(light_attenuation_linear, att.linear);
        shader.set_uniform(light_attenuation_quadratic, att.quadratic);

        shader.set_uniform(light_type, static_cast<int>(light.light_type));
        shader.set_uniform(view_position, camera);
    }

    void
    MaterialShader::set_colors
    (
        const core::Rgb& ambient_data,
        const core::Rgb& diffuse_data,
        const core::Rgb& specular_data,
        float shininess_data
    )
    {
        if(!ambient.is_null())
        {
            shader.set_uniform(ambient, ambient_data);
        }

        if(!diffuse.is_null())
        {
            shader.set_uniform(diffuse, diffuse_data);
        }

        if(!specular.is_null())
        {
            const auto the_specular = shininess_data > 0
                ? specular_data
                : core::Rgb {core::NamedColor::black}
                ;
            shader.set_uniform(specular, the_specular);
        }

        if(!shininess.is_null())
        {
            // todo(Gustav): change to 0 instead of 1.0
            const auto the_shininess = shininess_data > 0 ? shininess_data : 1.0f;
            shader.set_uniform(shininess, the_shininess);
        }
    }

}
