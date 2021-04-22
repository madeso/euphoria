#include "render/buffer2d.h"

#include "core/assert.h"
#include "core/bufferbuilder2d.h"
#include "core/cint.h"

#include "render/shaderattribute2d.h"

namespace euphoria::render
{
    Buffer2d::Buffer2d(const core::buffer_builder2d& bb)
        : index_count(core::Csizet_to_int(bb.tris.size()))
    {
        PointLayout::Bind(&vao);
        VertexBuffer::Bind(&vbo);

        vbo.SetData(bb.data);

        IndexBuffer::Bind(&ebo);
        ebo.SetData(bb.tris);

        vao.BindData(attributes2d::Vertex(), sizeof(float) * 4, 0);

        PointLayout::Bind(nullptr);

        IndexBuffer::Bind(nullptr);
        VertexBuffer::Bind(nullptr);
    }


    void
    Buffer2d::Draw() const
    {
        IndexBuffer::Bind(&ebo);
        PointLayout::Bind(&vao);
        ebo.Draw(RenderMode::Triangles, index_count);
        PointLayout::Bind(nullptr);
        IndexBuffer::Bind(nullptr);
    }

}
