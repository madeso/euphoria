#pragma once

#include <vector>

#include "core/enum.h"
#include "core/vfs_path.h"

#include "render/shader.h"

namespace euphoria::render
{
    struct light;

    struct material_shader_default_texture
    {
    public:
        material_shader_default_texture
        (
            const core::enum_value& a_name,
            const core::vfs::file_path& a_path
        );

        core::enum_value name;
        core::vfs::file_path path;
    };

    struct material_shader_binding
    {
        material_shader_binding
        (
                shader_uniform a_uniform,
                const core::enum_value& a_name
        );

        shader_uniform uniform;
        core::enum_value name;
    };

    /** Extends a regular Shader with uniform bindings for 3d rendering
     */
    struct material_shader
    {
    public:
        material_shader();

        bool
        load
        (
            core::vfs::file_system* file_system,
            const core::vfs::file_path& path
        );

        void
        use_shader();

        void
        set_projection(const core::mat4f& projection_data);

        void
        set_view(const core::mat4f& view_data);

        void
        set_model(const core::mat4f& model_data);

        void
        setup_light(const light& light, const core::vec3f& camera);

        void
        set_colors
        (
            const core::rgb& ambient_data,
            const core::rgb& diffuse_data,
            const core::rgb& specular_data,
            float shininess_data
        );

        render::shader shader;

        // private:
        shader_uniform projection;
        shader_uniform view;
        shader_uniform model;

        shader_uniform ambient;
        shader_uniform diffuse;
        shader_uniform specular;
        shader_uniform shininess;

        bool has_light;
        shader_uniform light_ambient;
        shader_uniform light_diffuse;
        shader_uniform light_specular;
        shader_uniform light_position;
        shader_uniform light_direction;
        shader_uniform light_type;
        shader_uniform light_attenuation_constant;
        shader_uniform light_attenuation_linear;
        shader_uniform light_attenuation_quadratic;
        shader_uniform light_cutoff_angle_outer;
        shader_uniform light_cutoff_angle_inner;

        shader_uniform normal_matrix;
        shader_uniform view_position;

        std::vector<material_shader_binding> bindings;
        std::vector<material_shader_default_texture> default_textures;
    };

}
