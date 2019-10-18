#include "render/buffer2d.h"

#include "core/assert.h"
#include "core/bufferbuilder2d.h"

#include "render/shaderattribute2d.h"

namespace euphoria::render
{
    Buffer2d::Buffer2d(const core::BufferBuilder2d& bb)
        : index_count_(bb.GetTriangleIndices().size())
    {
        Vao::Bind(&vao_);
        VertexBuffer::Bind(&vbo_);

        vbo_.SetData(bb.GetVertexData());

        IndexBuffer::Bind(&ebo_);
        ebo_.SetData(bb.GetTriangleIndices());

        vao_.BindVboData(attributes2d::Vertex(), sizeof(float) * 4, 0);

        Vao::Bind(nullptr);

        IndexBuffer::Bind(nullptr);
        VertexBuffer::Bind(nullptr);
    }

    void
    Buffer2d::Draw() const
    {
        Vao::Bind(&vao_);
        ebo_.Draw(index_count_);
        Vao::Bind(nullptr);
    }

}  // namespace euphoria::render
