#pragma once

#include <string>
#include <vector>

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
    struct texture2d;


    struct shader_id
    {
    public:
        shader_id();
        ~shader_id();

        NONCOPYABLE(shader_id);

        [[nodiscard]] bool
        is_currently_bound() const;

        [[nodiscard]] gluint
        get_id() const;

    private:
        gluint id;
    };


    struct shader : public shader_id
    {
    public:
        shader();
        ~shader() = default;

        NONCOPYABLE(shader);

        void
        pre_bind(const shader_attribute& attribute);

        bool
        load(core::vfs::FileSystem* fs, const core::vfs::FilePath& file_path);

        bool
        compile
        (
            const glchar* vertex_source,
            const glchar* fragment_source,
            const glchar* geometry_source = nullptr
        );

        // uniform = shader global
        shader_uniform
        get_uniform(const std::string& name);

        void
        set_uniform(const shader_uniform& attribute, glint val);

        void
        set_uniform(const shader_uniform& attribute, float val);

        void
        set_uniform(const shader_uniform& attribute, const core::Rgb& val);

        void
        set_uniform(const shader_uniform& attribute, const core::Rgba& val);

        void
        set_uniform(const shader_uniform& attribute, const core::Vec3f& val);

        void
        set_uniform(const shader_uniform& attribute, const core::Vec4f& val);

        void
        set_uniform(const shader_uniform& attribute, const core::Mat3f& val);

        void
        set_uniform(const shader_uniform& attribute, const core::mat4f& val);

        void
        set_uniform(const shader_uniform& attribute, const core::Rectf& val);

        // debug
        [[nodiscard]] static const shader*
        get_current_bound_for_debug();

        [[nodiscard]] const std::vector<shader_attribute>&
        get_attributes() const;

        [[nodiscard]] const core::vfs::FilePath&
        get_name() const;

    private:
        [[nodiscard]] bool
        has_bound_attribute(const shader_attribute& attribute) const;

        [[nodiscard]] bool
        has_bound_uniform(const shader_uniform& uniform) const;

        std::vector<shader_attribute> bound_attributes;
        std::vector<shader_uniform> bound_uniforms;
        core::vfs::FilePath shader_name;
    };

    void
    use(const render::shader* shader);

    void
    bind_texture_to_shader
    (
            texture2d* texture,
            shader* shader,
            const shader_uniform& attribute,
            glint index
    );

}
