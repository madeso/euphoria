#include "render/shaderattribute.h"

ShaderAttribute::ShaderAttribute(const std::string& aname, glint aid,
                                 ShaderAttributeSize asize)
    : name(aname)
    , id(aid)
    , size(asize)
{
}

bool
operator==(const ShaderAttribute& lhs, const ShaderAttribute& rhs)
{
  return lhs.name == rhs.name && lhs.id == rhs.id && lhs.size == rhs.size;
}
