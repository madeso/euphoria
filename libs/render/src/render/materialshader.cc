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
    material_shader_default_texture::material_shader_default_texture
    (
        const core::enum_value& a_name,
        const core::vfs::file_path& a_path
    )
        : name(a_name)
        , path(a_path)
    {
    }

    ////////////////////////////////////////////////////////////////////////////////

    material_shader_binding::material_shader_binding
    (
        shader_uniform a_uniform,
        const core::enum_value& a_name
    )
        : uniform(std::move(a_uniform))
        , name(a_name)
    {
    }


    ////////////////////////////////////////////////////////////////////////////////

    material_shader::material_shader()
        : projection(shader_uniform::null())
        , view(shader_uniform::null())
        , model(shader_uniform::null())
        , ambient(shader_uniform::null())
        , diffuse(shader_uniform::null())
        , specular(shader_uniform::null())
        , shininess(shader_uniform::null())
        , has_light(false)
        , light_ambient(shader_uniform::null())
        , light_diffuse(shader_uniform::null())
        , light_specular(shader_uniform::null())
        , light_position(shader_uniform::null())
        , light_direction(shader_uniform::null())
        , light_type(shader_uniform::null())
        , light_attenuation_constant(shader_uniform::null())
        , light_attenuation_linear(shader_uniform::null())
        , light_attenuation_quadratic(shader_uniform::null())
        , light_cutoff_angle_outer(shader_uniform::null())
        , light_cutoff_angle_inner(shader_uniform::null())
        , normal_matrix(shader_uniform::null())
        , view_position(shader_uniform::null())
    {
    }

    void
    PostBuild
    (
        material_shader* sh,
        const materialshader::MaterialShader& file,
        const core::vfs::file_path& path
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
            const auto texture_path = core::vfs::file_path::from_script(texture.path);
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
        sh->view       = sh->shader.get_uniform("uView");
        sh->model      = sh->shader.get_uniform("uModel");

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
            sh->light_ambient   = sh->shader.get_uniform("uLight.ambient");
            sh->light_diffuse   = sh->shader.get_uniform("uLight.diffuse");
            sh->light_specular  = sh->shader.get_uniform("uLight.specular");
            sh->light_position  = sh->shader.get_uniform("uLight.position");
            sh->light_direction = sh->shader.get_uniform("uLight.direction");
            sh->light_type      = sh->shader.get_uniform("uLight.type");
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
    material_shader::load
    (
        core::vfs::file_system* file_system,
        const core::vfs::file_path& path
    )
    {
        attributes3d::prebind_shader(&shader);
        const bool shader_compile = shader.load(file_system, path);
        // if (!shader_compile) { return false; }

        materialshader::MaterialShader file;
        const auto proto_path = path.set_extension_copy("json");
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
    material_shader::use_shader()
    {
        use(&shader);
    }

    void
    material_shader::set_projection(const core::mat4f& projection_data)
    {
        shader.set_uniform(projection, projection_data);
    }

    void
    material_shader::set_view(const core::mat4f& view_data)
    {
        shader.set_uniform(view, view_data);
    }

    void
    material_shader::set_model(const core::mat4f& model_data)
    {
        shader.set_uniform(model, model_data);
        if(has_light)
        {
            core::mat4f normal   = model_data;
            const bool  inverted = normal.invert();
            ASSERT(inverted);
            normal = normal.get_transposed();
            shader.set_uniform(normal_matrix, normal.get_mat3());
        }
    }

    void
    material_shader::setup_light(const light& light, const core::vec3f& camera)
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
    material_shader::set_colors
    (
        const core::rgb& ambient_data,
        const core::rgb& diffuse_data,
        const core::rgb& specular_data,
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
                : core::rgb {core::color::black}
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
