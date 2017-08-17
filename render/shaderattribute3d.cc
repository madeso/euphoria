#include "render/shaderattribute3d.h"

#include "core/assert.h"

#include "render/shader.h"

namespace attributes3d
{
  const ShaderAttribute&
  Vertex()
  {
    static ShaderAttribute s_Attribute{"aPosition", 1,
                                       ShaderAttributeSize::VEC3};
    return s_Attribute;
  }

  const ShaderAttribute&
  Normal()
  {
    static ShaderAttribute s_Attribute{"aNormal", 2, ShaderAttributeSize::VEC3};
    return s_Attribute;
  }

  const ShaderAttribute&
  TexCoord()
  {
    static ShaderAttribute s_Attribute{"aTexCoord", 3,
                                       ShaderAttributeSize::VEC2};
    return s_Attribute;
  }

  void
  PrebindShader(Shader* shader)
  {
    Assert(shader);
    shader->PreBind(Vertex());
    shader->PreBind(Normal());
    shader->PreBind(TexCoord());
  }
}  // namespace attributes3d
