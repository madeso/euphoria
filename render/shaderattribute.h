#ifndef RENDER_SHADERATTRIBUTE_H
#define RENDER_SHADERATTRIBUTE_H

#include <string>

#include "render/gltypes.h"

enum class ShaderAttributeSize {
  VEC1 = 1,
  VEC2 = 2,
  VEC3 = 3,
  VEC4 = 4,
  MAT44 = 16
};

class ShaderAttribute {
 public:
  ShaderAttribute(const std::string& name, gluint id, ShaderAttributeSize size);

  std::string name;
  gluint id;
  ShaderAttributeSize size;
};

#endif  // RENDER_SHADERATTRIBUTE_H
