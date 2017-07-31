#include "render/shaderattribute3d.h"

#include "core/assert.h"

#include "render/shader.h"

namespace attributes3d {

const ShaderAttribute& Vertex() {
  static ShaderAttribute attribute{"vertex", 1, ShaderAttributeSize::VEC3};
  return attribute;
}

const ShaderAttribute& TexCoord() {
  static ShaderAttribute attribute{"texcoord", 4, ShaderAttributeSize::VEC2};
  return attribute;
}

void PrebindShader(Shader* shader) {
  Assert(shader);
  shader->PreBind(Vertex());
  shader->PreBind(TexCoord());
}

}
