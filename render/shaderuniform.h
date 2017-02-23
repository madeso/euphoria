#ifndef RENDER_SHADERUNIFORM_H
#define RENDER_SHADERUNIFORM_H

#include <string>

#include "render/gltypes.h"

class Shader;

class ShaderUniform {
 public:
  ShaderUniform(const std::string& name, glint id, Shader* shader);

  std::string name;
  glint id;
  Shader* shader;
};

bool operator==(const ShaderUniform& lhs, const ShaderUniform& rhs);

#endif  // RENDER_SHADERUNIFORM_H
