#include "render/shaderattribute3d.h"

#include "core/assert.h"

#include "render/shader.h"

namespace attributes3d
{
  const ShaderAttribute&
  Vertex()
  {
    static ShaderAttribute sAttribute{"aPosition", 1, ShaderAttributeSize::VEC3};
    return sAttribute;
  }

  const ShaderAttribute&
  Normal()
  {
    static ShaderAttribute sAttribute{"aNormal", 2, ShaderAttributeSize::VEC3};
    return sAttribute;
  }

  const ShaderAttribute&
  TexCoord()
  {
    static ShaderAttribute sAttribute{"aTexCoord", 3, ShaderAttributeSize::VEC2};
    return sAttribute;
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
