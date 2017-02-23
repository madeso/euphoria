#include "render/shaderattribute2d.h"

#include "core/assert.h"

#include "render/shader.h"

namespace attributes2d {

const ShaderAttribute& Vertex() {
  static ShaderAttribute attribute{"vertex", 1, ShaderAttributeSize::VEC4};
  return attribute;
}

void PrebindShader(Shader* shader) {
  Assert(shader);
  shader->PreBind(Vertex());
}

}