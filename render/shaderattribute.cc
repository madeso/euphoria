#include "render/shaderattribute.h"

#include <utility>

ShaderAttribute::ShaderAttribute(
    std::string aname, glint aid, ShaderAttributeSize asize)
    : name(std::move(aname))
    , id(aid)
    , size(asize)
{
}

bool
operator==(const ShaderAttribute& lhs, const ShaderAttribute& rhs)
{
  return lhs.name == rhs.name && lhs.id == rhs.id && lhs.size == rhs.size;
}
