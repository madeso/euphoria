#include "render/materialshader.h"

#include <utility>

#include "log/log.h"
#include "core/texturetypes.h"

#include "render/shaderattribute3d.h"
#include "render/light.h"

#include "files/materialshader.h"



namespace eu::render
{
    MaterialShaderDefaultTexture::MaterialShaderDefaultTexture
    (
        const core::EnumValue& a_name,
        const io::FilePath& a_path
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
        : projection(ShaderUniform::create_null())
        , view(ShaderUniform::create_null())
        , model(ShaderUniform::create_null())
        , ambient(ShaderUniform::create_null())
        , diffuse(ShaderUniform::create_null())
        , specular(ShaderUniform::create_null())
        , shininess(ShaderUniform::create_null())
        , has_light(false)
        , light_ambient(ShaderUniform::create_null())
        , light_diffuse(ShaderUniform::create_null())
        , light_specular(ShaderUniform::create_null())
        , light_position(ShaderUniform::create_null())
        , light_direction(ShaderUniform::create_null())
        , light_type(ShaderUniform::create_null())
        , light_attenuation_constant(ShaderUniform::create_null())
        , light_attenuation_linear(ShaderUniform::create_null())
        , light_attenuation_quadratic(ShaderUniform::create_null())
        , light_cutoff_angle_outer(ShaderUniform::create_null())
        , light_cutoff_angle_inner(ShaderUniform::create_null())
        , normal_matrix(ShaderUniform::create_null())
        , view_position(ShaderUniform::create_null())
    {
    }

    void
    post_build
    (
        MaterialShader* sh,
        const files::materialshader::MaterialShader& file,
        const io::FilePath& path
    )
    {
        sh->has_light = file.has_light;

        for(const auto& texture: file.textures)
        {
            const auto uniform = sh->shader.get_uniform(texture.uniform);
            DEFINE_ENUM_VALUE(core::texture_type, texture_name, texture.texture);
            sh->bindings.emplace_back(uniform, texture_name);
        }

        for(const auto& texture: file.default_textures)
        {
            const auto texture_path = io::FilePath::from_script(texture.path);
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

        if(file.ambient)
        {
            sh->ambient = sh->shader.get_uniform(*file.ambient);
        }
        if(file.diffuse)
        {
            sh->diffuse = sh->shader.get_uniform(*file.diffuse);
        }
        if(file.specular)
        {
            sh->specular = sh->shader.get_uniform(*file.specular);
        }
        if(file.shininess)
        {
            sh->shininess = sh->shader.get_uniform(*file.shininess);
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
        io::FileSystem* file_system,
        const io::FilePath& path
    )
    {
        attributes3d::add_attributes_to_shader(&shader);
        const bool shader_compile = shader.load(file_system, path);
        // if (!shader_compile) { return false; }

        const auto proto_path = path.set_extension_copy("json");
        
        files::materialshader::MaterialShader file;
        if (const auto loaded = io::read_json_file(file_system, proto_path); loaded == false)
        {
            LOG_ERROR("Failed to load {}: {}", proto_path, loaded.get_error().display);
            return false;
        }
        else
        {
            const auto& json = loaded.get_value();
            const auto parsed = files::materialshader::parse(log::get_global_logger(), &file, json.root, &json.doc);
            if (!parsed)
            {
                return false;
            }
        }

        // todo(Gustav): set default shader names if file failed to load

        post_build(this, file, path);

        return shader_compile;
    }

    void
    MaterialShader::use_shader()
    {
        use(&shader);
    }

    void
    MaterialShader::set_projection(const core::mat4f& projection_data)
    {
        shader.set_uniform(projection, projection_data);
    }

    void
    MaterialShader::set_view(const core::mat4f& view_data)
    {
        shader.set_uniform(view, view_data);
    }

    void
    MaterialShader::set_model(const core::mat4f& model_data)
    {
        shader.set_uniform(model, model_data);
        if(has_light)
        {
            core::mat4f normal = model_data;
            const bool inverted = normal.invert();
            ASSERT(inverted);
            normal = normal.get_transposed();
            shader.set_uniform(normal_matrix, normal.get_mat3());
        }
    }

    void
    MaterialShader::setup_light(const Light& light, const core::vec3f& camera)
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
