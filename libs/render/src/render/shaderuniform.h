#pragma once

#include <string>

#include "core/noncopyable.h"

#include "render/gltypes.h"


namespace euphoria::render
{
    struct shader;

    struct shader_uniform
    {
    public:
        shader_uniform(std::string aname, glint aid, render::shader* ashader);

        ~shader_uniform() = default;
        shader_uniform(const shader_uniform&) = default;
        shader_uniform(shader_uniform&&) = default;
        shader_uniform& operator=(const shader_uniform&) = default;
        shader_uniform& operator=(shader_uniform&&) = default;

        [[nodiscard]] static const shader_uniform&
        null();

        [[nodiscard]] bool
        is_null() const;

        std::string name;
        glint id;
        render::shader* shader;

    private:
        shader_uniform(); // creates a null shader
    };

    bool
    operator==(const shader_uniform& lhs, const shader_uniform& rhs);
}
