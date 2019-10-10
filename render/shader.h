#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <vector>

#include "core/vec2.h"
#include "core/vec3.h"
#include "core/vec4.h"
#include "core/rgb.h"
#include "core/mat4.h"
#include "core/rect.h"

#include "render/gltypes.h"
#include "core/noncopyable.h"

#include "render/shaderattribute.h"
#include "render/shaderuniform.h"

namespace euphoria::core
{
    namespace vfs
    {
        struct FileSystem;
    }
}  // namespace euphoria::core

namespace euphoria::render
{
    struct ShaderId : core::Noncopyable
    {
    public:
        ShaderId();
        ~ShaderId();

        bool
        IsCurrentlyBound() const;

        gluint
        GetId() const;

    private:
        gluint id_;
    };

    void
    Use(const Shader* shader);

    struct Shader : public ShaderId
    {
    public:
        Shader();

        // shader attribute =
        void
        PreBind(const ShaderAttribute& attribute);
        bool
        Load(core::vfs::FileSystem* fs, const std::string& file_path);

        bool
        Compile(const glchar* vertex_source,
                const glchar* fragment_source,
                const glchar* geometry_source = nullptr);

    public:
        // uniform = shader global
        ShaderUniform
        GetUniform(const std::string& name);

        void
        SetUniform(const ShaderUniform& attribute, glint val);
        void
        SetUniform(const ShaderUniform& attribute, float val);
        void
        SetUniform(const ShaderUniform& attribute, const core::Rgb& val);
        void
        SetUniform(const ShaderUniform& attribute, const core::Rgba& val);
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
        static const Shader*
        CurrentlyBound();
        const std::vector<ShaderAttribute>&
        GetAttributes() const;
        const std::string&
        GetName() const;

    private:
        // debug
        std::vector<ShaderAttribute> bound_attributes_;
        bool
                                   HasBoundAttribute(const ShaderAttribute& attribute) const;
        std::vector<ShaderUniform> bound_uniforms_;
        bool
                    HasBoundUniform(const ShaderUniform& uniform) const;
        std::string shader_name_;
    };

    struct Texture2d;
    void
    BindTextureToShader(
            Texture2d*           texture,
            Shader*              shader,
            const ShaderUniform& attribute,
            glint                index);

}  // namespace euphoria::render

#endif
