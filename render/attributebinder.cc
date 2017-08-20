#include "attributebinder.h"

#include "render/compiledmesh.h"

void
AttributeBinder::Register(const ShaderAttribute& attribute)
{
  const int size = sizeof(float) * static_cast<int>(attribute.size);
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
