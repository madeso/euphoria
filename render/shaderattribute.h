#ifndef RENDER_SHADERATTRIBUTE_H
#define RENDER_SHADERATTRIBUTE_H

#include <string>

#include "render/gltypes.h"

enum class AttributeSize {
  VEC1 = 1,
  VEC2 = 2,
  VEC3 = 3,
  VEC4 = 4
};

class Attribute {
 public:
  std::string name;
  glid id;
  AttributeSize size;
};

#endif  // RENDER_SHADERATTRIBUTE_H
