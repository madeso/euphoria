#ifndef RENDER_SHADERUNIFORM_H
#define RENDER_SHADERUNIFORM_H

#include <string>

#include "render/gltypes.h"

namespace euphoria::render
{
    class Shader;

    class ShaderUniform
    {
    public:
        ShaderUniform(const ShaderUniform&) = default;
        ShaderUniform(std::string aname, glint aid, Shader* ashader);

        static const ShaderUniform&
        Null();

        bool
        IsNull() const;

        std::string name;
        glint       id;
        Shader*     shader;

    private:
        ShaderUniform();  // creates a null shader
    };

    bool
    operator==(const ShaderUniform& lhs, const ShaderUniform& rhs);
}  // namespace euphoria::render

#endif  // RENDER_SHADERUNIFORM_H
