#pragma once

#include <string>

#include "core/noncopyable.h"

#include "render/gltypes.h"


namespace euphoria::render
{
    struct Shader;

    struct ShaderUniform
    {
    public:
        ShaderUniform(std::string aname, glint aid, Shader* ashader);

        ~ShaderUniform() = default;
        ShaderUniform(const ShaderUniform&) = default;
        ShaderUniform(ShaderUniform&&) = default;
        ShaderUniform& operator=(const ShaderUniform&) = default;
        ShaderUniform& operator=(ShaderUniform&&) = default;

        [[nodiscard]] static const ShaderUniform&
        Null();

        [[nodiscard]] bool
        IsNull() const;

        std::string name;
        glint id;
        Shader* shader;

    private:
        ShaderUniform(); // creates a null shader
    };

    bool
    operator==(const ShaderUniform& lhs, const ShaderUniform& rhs);
}
