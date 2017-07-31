#ifndef EUPHORIA_SHADERATTRIBUTE3D_H
#define EUPHORIA_SHADERATTRIBUTE3D_H


#include "render/shaderattribute.h"

class Shader;

namespace attributes3d {

const ShaderAttribute& Vertex();
const ShaderAttribute& TexCoord();

void PrebindShader(Shader* shader);
}


#endif //EUPHORIA_SHADERATTRIBUTE3D_H
