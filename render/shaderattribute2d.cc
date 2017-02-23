#include "render/shaderattribute2d.h"

#include "core/assert.h"

#include "render/shader.h"

namespace attributes2d {

/*
 LAYOUT:
 unassigned | image | projection | model   | color | rgba
 0          | 1     | 2 3 4 5    | 6 7 8 9 | 10    | 11

*/

const ShaderAttribute& Rgba() {
  static ShaderAttribute attribute{"rgba", 11, ShaderAttributeSize::VEC4};
  return attribute;
}

const ShaderAttribute& Color() {
  static ShaderAttribute attribute{"color", 10, ShaderAttributeSize::VEC4};
  return attribute;
}

const ShaderAttribute& Model() {
  static ShaderAttribute attribute{"model", 6, ShaderAttributeSize::MAT44};
  return attribute;
}

const ShaderAttribute& Projection() {
  static ShaderAttribute attribute{"projection", 2, ShaderAttributeSize::MAT44};
  return attribute;
}

const ShaderAttribute& Image() {
  static ShaderAttribute attribute{"image", 1, ShaderAttributeSize::VEC1};
  return attribute;
}

void PrebindShader(Shader* shader) {
  Assert(shader);
  shader->PreBind(Rgba());
  shader->PreBind(Color());
  shader->PreBind(Model());
  shader->PreBind(Image());
  shader->PreBind(Projection());
}

}