#include "render/shaderattribute2d.h"

#include "core/assert.h"

#include "render/shader.h"

namespace euphoria::render
{

namespace attributes2d
{
  const ShaderAttribute&
  Vertex()
  {
    static ShaderAttribute Attribute{"vertex", 1, ShaderAttributeSize::VEC4};
    return Attribute;
  }

  void
  PrebindShader(Shader* shader)
  {
    ASSERT(shader);
    shader->PreBind(Vertex());
  }
}  // namespace attributes2d

}
