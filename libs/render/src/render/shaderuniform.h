#pragma once

#include <string>

#include "core/noncopyable.h"

#include "render/gltypes.h"


namespace euphoria::render
{
    struct ShaderProgram;

    struct ShaderUniform
    {
    public:
        ShaderUniform(std::string aname, glint aid, render::ShaderProgram* ashader);

        ~ShaderUniform() = default;
        ShaderUniform(const ShaderUniform&) = default;
        ShaderUniform(ShaderUniform&&) = default;
        ShaderUniform& operator=(const ShaderUniform&) = default;
        ShaderUniform& operator=(ShaderUniform&&) = default;

        [[nodiscard]] static const ShaderUniform&
        null();

        [[nodiscard]] bool
        is_null() const;

        std::string name;
        glint id;
        render::ShaderProgram* shader;

    private:
        ShaderUniform(); // creates a null shader
    };

    bool
    operator==(const ShaderUniform& lhs, const ShaderUniform& rhs);
}
