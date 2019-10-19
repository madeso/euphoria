#ifndef EUPHORIA_SHADERATTRIBUTE3D_H
#define EUPHORIA_SHADERATTRIBUTE3D_H

#include "render/shaderattribute.h"

namespace euphoria::render
{
    struct Shader;

    // todo(Gustav): Refactor into a pipeline/layout structure
    /*
    Currently attributes are hardcoded into a special layout.
    It would be preferable if the layout is written in script
    and the actual values are dynamically created in script.
     */

    namespace attributes3d
    {
        const ShaderAttribute&
        Vertex();
        const ShaderAttribute&
        Normal();
        const ShaderAttribute&
        TexCoord();

        void
        PrebindShader(Shader* shader);
    }  // namespace attributes3d
}  // namespace euphoria::render

#endif  // EUPHORIA_SHADERATTRIBUTE3D_H
