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


    struct ShaderId
    {
    public:
        ShaderId();
        ~ShaderId();

        NONCOPYABLE(ShaderId);

        [[nodiscard]] bool
        IsCurrentlyBound() const;

        [[nodiscard]] gluint
        GetId() const;

    private:
        gluint id_;
    };


    struct Shader : public ShaderId
    {
    public:
        Shader();
        ~Shader() = default;

        NONCOPYABLE(Shader);

        void
        PreBind(const ShaderAttribute& attribute);

        bool
        Load(core::vfs::FileSystem* fs, const core::vfs::FilePath& file_path);

        bool
        Compile
        (
            const glchar* vertex_source,
            const glchar* fragment_source,
            const glchar* geometry_source = nullptr
        );

        // uniform = shader global
        ShaderUniform
        GetUniform(const std::string& name);

        void
        SetUniform(const ShaderUniform& attribute, glint val);

        void
        SetUniform(const ShaderUniform& attribute, float val);

        void
        SetUniform(const ShaderUniform& attribute, const core::rgb& val);

        void
        SetUniform(const ShaderUniform& attribute, const core::rgba& val);

        void
        SetUniform(const ShaderUniform& attribute, const core::vec3f& val);

        void
        SetUniform(const ShaderUniform& attribute, const core::vec4f& val);

        void
        SetUniform(const ShaderUniform& attribute, const core::mat3f& val);

        void
        SetUniform(const ShaderUniform& attribute, const core::mat4f& val);

        void
        SetUniform(const ShaderUniform& attribute, const core::Rectf& val);

        // debug
        [[nodiscard]] static const Shader*
        CurrentlyBound();

        [[nodiscard]] const std::vector<ShaderAttribute>&
        GetAttributes() const;

        [[nodiscard]] const core::vfs::FilePath&
        GetName() const;

    private:
        [[nodiscard]] bool
        HasBoundAttribute(const ShaderAttribute& attribute) const;

        [[nodiscard]] bool
        HasBoundUniform(const ShaderUniform& uniform) const;

        std::vector<ShaderAttribute> bound_attributes_;
        std::vector<ShaderUniform> bound_uniforms_;
        core::vfs::FilePath shader_name_;
    };

    void
    Use(const Shader* shader);

    void
    BindTextureToShader
    (
        texture2d* texture,
        Shader* shader,
        const ShaderUniform& attribute,
        glint index
    );

}
