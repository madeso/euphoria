#include "render/shaderuniform.h"

#include "core/assert.h"

#include <utility>

namespace euphoria::render
{
    shader_uniform::shader_uniform(std::string aname, glint aid, render::shader* ashader)
        : name(std::move(aname))
        , id(aid)
        , shader(ashader)
    {
    }


    const shader_uniform&
    shader_uniform::null()
    {
        static const auto null_uniform = shader_uniform{};
        ASSERT(null_uniform.is_null());
        return null_uniform;
    }


    bool
    shader_uniform::is_null() const
    {
        return shader == nullptr && id == 0;
    }


    bool
    operator==(const shader_uniform& lhs, const shader_uniform& rhs)
    {
        return
            lhs.id == rhs.id &&
            lhs.name == rhs.name &&
            lhs.shader == rhs.shader;
    }

    shader_uniform::shader_uniform()
        : id(0)
        , shader(nullptr)
    {
    }
}
