#ifndef RENDER_SHADERATTRIBUTE_H
#define RENDER_SHADERATTRIBUTE_H

#include <string>

#include "render/gltypes.h"

namespace euphoria::render
{
    enum class ShaderAttributeSize
    {
        VEC1  = 1,
        VEC2  = 2,
        VEC3  = 3,
        VEC4  = 4,
        MAT44 = 16
    };

    /** Respresents a shder attribute like vertex, normal or uv coord.
     */
    struct ShaderAttribute
    {
    public:
        ShaderAttribute(
                std::string         aname,
                glint               aid,
                ShaderAttributeSize asize);

        /// the name of the shader attribute 
        std::string         name;

        /// the id of the attribute
        glint               id;

        /// the size of the attribute
        ShaderAttributeSize size;
    };

    bool
    operator==(const ShaderAttribute& lhs, const ShaderAttribute& rhs);

}  // namespace euphoria::render

#endif  // RENDER_SHADERATTRIBUTE_H
