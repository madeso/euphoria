#pragma once




#include "core/vec2.h"
#include "core/vec3.h"
#include "core/vec4.h"
#include "core/rgb.h"
#include "core/mat4.h"
#include "core/rect.h"
#include "core/noncopyable.h"
#include "core/vfs_path.h"

#include "render/gltypes.h"
#include "render/shaderattribute.h"
#include "render/shaderuniform.h"


namespace euphoria::core::vfs
{
    struct FileSystem;
}


namespace euphoria::render
{
    struct Texture2;


    struct ShaderId
    {
    public:
        ShaderId();
        ~ShaderId();

        NONCOPYABLE(ShaderId);

        [[nodiscard]] bool
        is_currently_bound() const;

        [[nodiscard]] gl::Uint
        get_id() const;

    private:
        gl::Uint id;
    };


    struct ShaderProgram : public ShaderId
    {
    public:
        ShaderProgram();
        ~ShaderProgram() = default;

        NONCOPYABLE(ShaderProgram);

        void
        pre_bind(const ShaderAttribute& attribute);

        bool
        load(core::vfs::FileSystem* fs, const core::vfs::FilePath& file_path);

        bool
        compile
        (
            const gl::Char* vertex_source,
            const gl::Char* fragment_source,
            const gl::Char* geometry_source = nullptr
        );

        // uniform = shader global
        ShaderUniform
        get_uniform(const std::string& name);

        void
        set_uniform(const ShaderUniform& attribute, gl::Int val);

        void
        set_uniform(const ShaderUniform& attribute, float val);

        void
        set_uniform(const ShaderUniform& attribute, const core::rgb& val);

        void
        set_uniform(const ShaderUniform& attribute, const core::rgba& val);

        void
        set_uniform(const ShaderUniform& attribute, const core::vec3f& val);

        void
        set_uniform(const ShaderUniform& attribute, const core::vec4f& val);

        void
        set_uniform(const ShaderUniform& attribute, const core::mat3f& val);

        void
        set_uniform(const ShaderUniform& attribute, const core::mat4f& val);

        void
        set_uniform(const ShaderUniform& attribute, const core::Rectf& val);

        // debug
        [[nodiscard]] static const ShaderProgram*
        get_current_bound_for_debug();

        [[nodiscard]] const std::vector<ShaderAttribute>&
        get_attributes() const;

        [[nodiscard]] const core::vfs::FilePath&
        get_name() const;

    private:
        [[nodiscard]] bool
        has_bound_attribute(const ShaderAttribute& attribute) const;

        [[nodiscard]] bool
        has_bound_uniform(const ShaderUniform& uniform) const;

        std::vector<ShaderAttribute> bound_attributes;
        std::vector<ShaderUniform> bound_uniforms;
        core::vfs::FilePath shader_name;
    };

    void
    use(const render::ShaderProgram* shader);

    void
    bind_texture_to_shader
    (
            Texture2* texture,
            ShaderProgram* shader,
            const ShaderUniform& attribute,
            gl::Int index
    );

}
