#pragma once

#include "render/gltypes.h"


namespace eu::render
{
    struct ShaderProgram;

    struct ShaderUniform
    {
    public:
        ShaderUniform(std::string aname, gl::Int aid, render::ShaderProgram* ashader);

        ~ShaderUniform() = default;
        ShaderUniform(const ShaderUniform&) = default;
        ShaderUniform(ShaderUniform&&) = default;
        ShaderUniform& operator=(const ShaderUniform&) = default;
        ShaderUniform& operator=(ShaderUniform&&) = default;

        [[nodiscard]] static const ShaderUniform&
        create_null();

        [[nodiscard]] bool
        is_null() const;

        std::string name;
        gl::Int id;
        render::ShaderProgram* shader;

    private:
        ShaderUniform(); // creates a null shader
    };

    bool
    operator==(const ShaderUniform& lhs, const ShaderUniform& rhs);
}
