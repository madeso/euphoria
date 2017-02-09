#include "render/shaderattribute2d.h"

namespace attributes2d {

const ShaderAttribute& Texture() {
  static ShaderAttribute attribute{"texture", 0, ShaderAttributeSize::VEC1};
  return attribute;
}

const ShaderAttribute& Rgba() {
  static ShaderAttribute attribute{"rgba", 0, ShaderAttributeSize::VEC4};
  return attribute;
}

}