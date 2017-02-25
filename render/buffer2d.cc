#include "core/assert.h"

#include "render/buffer2d.h"
#include "render/bufferbuilder2d.h"
#include "render/shaderattribute2d.h"

Buffer2d::Buffer2d(const BufferBuilder2d& bb)
  : tri_count_(bb.GetTriangleIndices().size()/3)
{
  // 1. Bind Vertex Array Object
  Vao::Bind(&vao_);
  Vbo::Bind(&vbo_);

  // 2. Copy our vertices array in a vertex buffer for OpenGL to use
  vbo_.SetData(bb.GetVertexData());

  // 3. Copy our index array in a element buffer for OpenGL to use
  Ebo::Bind(&ebo_);
  ebo_.SetData(bb.GetTriangleIndices());

  // 3. Then set the vertex attributes pointers
  vao_.BindVboData(attributes2d::Vertex(), sizeof(float) * 4, 0);

  // 4. Unbind VAO (NOT the EBO)
  Vao::Bind(nullptr);

  Ebo::Bind(nullptr);
  Vbo::Bind(nullptr);
}

void Buffer2d::Draw() const {
  Assert(this);
  Ebo::Bind(&ebo_);
  ebo_.Draw(tri_count_);
  Ebo::Bind(nullptr);
}
