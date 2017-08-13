#include "render/shaderuniform.h"

#include <utility>

ShaderUniform::ShaderUniform(std::string aname, glint aid, Shader* ashader)
    : name(std::move(aname))
    , id(aid)
    , shader(ashader)
{
}

ShaderUniform
ShaderUniform::Null()
{
  return ShaderUniform{};
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
