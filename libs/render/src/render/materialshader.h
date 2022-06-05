#pragma once



#include "core/enum.h"
#include "core/vfs_path.h"

#include "render/shader.h"

namespace euphoria::render
{
    struct Light;

    struct MaterialShaderDefaultTexture
    {
    public:
        MaterialShaderDefaultTexture
        (
            const core::EnumValue& a_name,
            const core::vfs::FilePath& a_path
        );

        core::EnumValue name;
        core::vfs::FilePath path;
    };

    struct MaterialShaderBinding
    {
        MaterialShaderBinding
        (
            ShaderUniform a_uniform,
            const core::EnumValue& a_name
        );

        ShaderUniform uniform;
        core::EnumValue name;
    };

    /** Extends a regular Shader with uniform bindings for 3d rendering
     */
    struct MaterialShader
    {
    public:
        MaterialShader();

        bool
        load
        (
            core::vfs::FileSystem* file_system,
            const core::vfs::FilePath& path
        );

        void
        use_shader();

        void
        set_projection(const core::Mat4f& projection_data);

        void
        set_view(const core::Mat4f& view_data);

        void
        set_model(const core::Mat4f& model_data);

        void
        setup_light(const Light& light, const core::Vec3f& camera);

        void
        set_colors
        (
            const core::Rgb& ambient_data,
            const core::Rgb& diffuse_data,
            const core::Rgb& specular_data,
            float shininess_data
        );

        render::ShaderProgram shader;

        // private:
        ShaderUniform projection;
        ShaderUniform view;
        ShaderUniform model;

        ShaderUniform ambient;
        ShaderUniform diffuse;
        ShaderUniform specular;
        ShaderUniform shininess;

        bool has_light;
        ShaderUniform light_ambient;
        ShaderUniform light_diffuse;
        ShaderUniform light_specular;
        ShaderUniform light_position;
        ShaderUniform light_direction;
        ShaderUniform light_type;
        ShaderUniform light_attenuation_constant;
        ShaderUniform light_attenuation_linear;
        ShaderUniform light_attenuation_quadratic;
        ShaderUniform light_cutoff_angle_outer;
        ShaderUniform light_cutoff_angle_inner;

        ShaderUniform normal_matrix;
        ShaderUniform view_position;

        std::vector<MaterialShaderBinding> bindings;
        std::vector<MaterialShaderDefaultTexture> default_textures;
    };

}
