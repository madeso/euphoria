#pragma once


#include "base/vec2.h"
#include "base/vec3.h"
#include "base/vec4.h"
#include "base/rgb.h"
#include "base/mat4.h"
#include "base/rect.h"
#include "io/vfs_path.h"

#include "render/gltypes.h"
#include "render/shaderattribute.h"
#include "render/shaderuniform.h"


namespace eu::io
{
    struct FileSystem;
}


namespace eu::render
{
    struct Texture2;


    struct ShaderId
    {
    public:
        ShaderId();
        ~ShaderId();

        ShaderId(const ShaderId& other) = delete;
        void operator=(const ShaderId&) = delete;
        ShaderId(ShaderId&& other) = delete;
        void operator=(ShaderId&&) = delete;

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

        ShaderProgram(const ShaderProgram& other) = delete;
        void operator=(const ShaderProgram&) = delete;
        ShaderProgram(ShaderProgram&& other) = delete;
        void operator=(ShaderProgram&&) = delete;

        void add_attribute(const ShaderAttribute& attribute);

        bool load(io::FileSystem* fs, const io::FilePath& file_path);

        bool compile
        (
            const gl::Char* vertex_source,
            const gl::Char* fragment_source,
            const gl::Char* geometry_source = nullptr
        );

        // uniform = shader global
        ShaderUniform get_uniform(const std::string& name);

        void set_uniform(const ShaderUniform& attribute, gl::Int val);
        void set_uniform(const ShaderUniform& attribute, float val);
        void set_uniform(const ShaderUniform& attribute, const Rgb& val);
        void set_uniform(const ShaderUniform& attribute, const Rgba& val);
        void set_uniform(const ShaderUniform& attribute, const vec3f& val);
        void set_uniform(const ShaderUniform& attribute, const vec4f& val);
        void set_uniform(const ShaderUniform& attribute, const mat3f& val);
        void set_uniform(const ShaderUniform& attribute, const mat4f& val);
        void set_uniform(const ShaderUniform& attribute, const Rectf& val);

        // debug
        [[nodiscard]] static const ShaderProgram*
        get_current_bound_for_debug();

        [[nodiscard]] const std::vector<ShaderAttribute>&
        get_attributes() const;

        [[nodiscard]] const io::FilePath&
        get_name() const;

    private:
        [[nodiscard]] bool
        has_bound_attribute(const ShaderAttribute& attribute) const;

        [[nodiscard]] bool
        has_bound_uniform(const ShaderUniform& uniform) const;

        std::vector<ShaderAttribute> bound_attributes;
        std::vector<ShaderUniform> bound_uniforms;
        io::FilePath shader_name;
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
