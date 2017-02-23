#ifndef RENDER_SHADERATTRIBUTE2D_H
#define RENDER_SHADERATTRIBUTE2D_H

#include "render/shaderattribute.h"

class Shader;

namespace attributes2d {

const ShaderAttribute& Rgba();
const ShaderAttribute& Color();
const ShaderAttribute& Model();
const ShaderAttribute& Image();
const ShaderAttribute& Projection();

void PrebindShader(Shader* shader);
}

#endif  // RENDER_SHADERATTRIBUTE2D_H
