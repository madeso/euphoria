#include "core/assert.h"

#include "render/buffer2d.h"
#include "render/bufferbuilder2d.h"
#include "render/shaderattribute2d.h"

Buffer2d::Buffer2d(const BufferBuilder2d& bb)
  : tri_count_(bb.GetTriangleIndices().size()/3)
{
  Vao::Bind(&vao_);
  Vbo::Bind(&vbo_);

  vbo_.SetData(bb.GetVertexData());

  Ebo::Bind(&ebo_);
  ebo_.SetData(bb.GetTriangleIndices());

  vao_.BindVboData(attributes2d::Vertex(), sizeof(float) * 4, 0);

  Vao::Bind(nullptr);

  Ebo::Bind(nullptr);
  Vbo::Bind(nullptr);
}

void Buffer2d::Draw() const {
  Assert(this);
  Vao::Bind(&vao_);
  ebo_.Draw(tri_count_);
  Vao::Bind(nullptr);
}
