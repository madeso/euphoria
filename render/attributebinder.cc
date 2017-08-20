#include "attributebinder.h"

#include "render/compiledmesh.h"

int
GetCount(ShaderAttributeSize size)
{
  switch(size)
  {
    case ShaderAttributeSize::VEC1:
      return 1;
    case ShaderAttributeSize::VEC2:
      return 2;
    case ShaderAttributeSize::VEC3:
      return 3;
    case ShaderAttributeSize::VEC4:
      return 4;
    case ShaderAttributeSize::MAT44:
      return 4 * 4;
  }
  DIE("Unhandled case");
  return 1;
}

void
AttributeBinder::Register(const ShaderAttribute& attribute)
{
  const int single_size = sizeof(float);
  const int size        = single_size * GetCount(attribute.size);
  bind_datas_.emplace_back(BindData{attribute, size});
  total_size_ += size;
}

void
AttributeBinder::Bind(const std::shared_ptr<CompiledMeshPart>& part)
{
  int stride = 0;
  for(const auto& d : bind_datas_)
  {
    part->config.BindVboData(d.attribute, total_size_, stride);
    stride += d.size;
  }
}

AttributeBinder::BindData::BindData(ShaderAttribute a, int s)
    : attribute(std::move(a))
    , size(s)
{
}
