#ifndef EUPHORIA_SHADERATTRIBUTE3D_H
#define EUPHORIA_SHADERATTRIBUTE3D_H

#include "render/shaderattribute.h"

namespace euphoria::render
{
    class Shader;

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
