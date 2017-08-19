#include "render/shaderuniform.h"

#include "core/assert.h"

#include <utility>

ShaderUniform::ShaderUniform(std::string aname, glint aid, Shader* ashader)
    : name(std::move(aname))
    , id(aid)
    , shader(ashader)
{
}

const ShaderUniform&
ShaderUniform::Null()
{
  static ShaderUniform s_Null{};
  ASSERT(s_Null.IsNull());
  return s_Null;
}

bool
ShaderUniform::IsNull() const
{
  return shader == nullptr && id == 0;
}


bool
operator==(const ShaderUniform& lhs, const ShaderUniform& rhs)
{
  return lhs.id == rhs.id && lhs.name == rhs.name && lhs.shader == rhs.shader;
}

ShaderUniform::ShaderUniform()
    : name("")
    , id(0)
    , shader(nullptr)
{
}
