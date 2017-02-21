#include "render/shaderattribute2d.h"

#include <cassert>

#include "render/shader.h"

namespace attributes2d {

const ShaderAttribute& Texture() {
  static ShaderAttribute attribute{"texture", 0, ShaderAttributeSize::VEC1};
  return attribute;
}

const ShaderAttribute& Rgba() {
  static ShaderAttribute attribute{"rgba", 0, ShaderAttributeSize::VEC4};
  return attribute;
}

const ShaderAttribute& Color() {
  static ShaderAttribute attribute{"color", 0, ShaderAttributeSize::VEC4};
  return attribute;
}

const ShaderAttribute& Model() {
  static ShaderAttribute attribute{"model", 0, ShaderAttributeSize::MAT44};
  return attribute;
}

const ShaderAttribute& Projection() {
  static ShaderAttribute attribute{"projection", 0, ShaderAttributeSize::MAT44};
  return attribute;
}

const ShaderAttribute& Image() {
  static ShaderAttribute attribute{"image", 0, ShaderAttributeSize::VEC1};
  return attribute;
}

void PrebindShader(Shader* shader) {
  assert(shader);
  shader->PreBind(Texture());
  shader->PreBind(Rgba());
  shader->PreBind(Color());
  shader->PreBind(Model());
  shader->PreBind(Image());
  shader->PreBind(Projection());
}

}