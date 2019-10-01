#include "render/shaderattribute3d.h"

#include "core/assert.h"

#include "render/shader.h"

namespace euphoria::render
{
  namespace attributes3d
  {
    const ShaderAttribute&
    Vertex()
    {
      static ShaderAttribute Attribute{"aPosition", 1, ShaderAttributeSize::VEC3};
      return Attribute;
    }

    const ShaderAttribute&
    Normal()
    {
      static ShaderAttribute Attribute{"aNormal", 2, ShaderAttributeSize::VEC3};
      return Attribute;
    }

    const ShaderAttribute&
    TexCoord()
    {
      static ShaderAttribute Attribute{"aTexCoord", 3, ShaderAttributeSize::VEC2};
      return Attribute;
    }

    void
    PrebindShader(Shader* shader)
    {
      ASSERT(shader);
      shader->PreBind(Vertex());
      shader->PreBind(Normal());
      shader->PreBind(TexCoord());
    }
  }  // namespace attributes3d
}
