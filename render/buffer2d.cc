#include "render/buffer2d.h"

#include "core/assert.h"
#include "core/bufferbuilder2d.h"

#include "render/shaderattribute2d.h"

namespace euphoria::render
{
    Buffer2d::Buffer2d(const core::BufferBuilder2d& bb)
        : index_count_(bb.GetTriangleIndices().size())
    {
        PointLayout::Bind(&vao_);
        VertexBuffer::Bind(&vbo_);

        vbo_.SetData(bb.GetVertexData());

        IndexBuffer::Bind(&ebo_);
        ebo_.SetData(bb.GetTriangleIndices());

        vao_.BindData(attributes2d::Vertex(), sizeof(float) * 4, 0);

        PointLayout::Bind(nullptr);

        IndexBuffer::Bind(nullptr);
        VertexBuffer::Bind(nullptr);
    }

    void
    Buffer2d::Draw() const
    {
        DIE("Hrm... using triangles doesn't look right here. Investigate!");
        PointLayout::Bind(&vao_);
        ebo_.Draw(RenderMode::Triangles, index_count_);
        PointLayout::Bind(nullptr);
    }

}  // namespace euphoria::render
