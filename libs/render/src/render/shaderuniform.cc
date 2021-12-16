#include "render/shaderuniform.h"

#include "core/assert.h"

#include <utility>

namespace euphoria::render
{
    ShaderUniform::ShaderUniform(std::string aname, OpenglInt aid, render::ShaderProgram* ashader)
        : name(std::move(aname))
        , id(aid)
        , shader(ashader)
    {
    }


    const ShaderUniform&
    ShaderUniform::null()
    {
        static const auto null_uniform = ShaderUniform{};
        ASSERT(null_uniform.is_null());
        return null_uniform;
    }


    bool
    ShaderUniform::is_null() const
    {
        return shader == nullptr && id == 0;
    }


    bool
    operator==(const ShaderUniform& lhs, const ShaderUniform& rhs)
    {
        return
            lhs.id == rhs.id &&
            lhs.name == rhs.name &&
            lhs.shader == rhs.shader;
    }

    ShaderUniform::ShaderUniform()
        : id(0)
        , shader(nullptr)
    {
    }
}
