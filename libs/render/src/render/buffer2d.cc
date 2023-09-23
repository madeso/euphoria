#include "render/buffer2d.h"

#include "assert/assert.h"
#include "core/bufferbuilder2d.h"
#include "base/cint.h"

#include "render/shaderattribute2d.h"

namespace eu::render
{
    Buffer2d::Buffer2d(const core::BufferBuilder2& bb)
        : index_count(c_sizet_to_int(bb.tris.size()))
    {
        PointLayout::bind(&vao);
        VertexBuffer::bind(&vbo);

        vbo.set_data(bb.data);

        IndexBuffer::bind(&ebo);
        ebo.set_data(bb.tris);

        vao.bind_data(attributes2d::vertex, sizeof(float) * 4, 0);

        PointLayout::bind(nullptr);

        IndexBuffer::bind(nullptr);
        VertexBuffer::bind(nullptr);
    }


    void
    Buffer2d::draw() const
    {
        IndexBuffer::bind(&ebo);
        PointLayout::bind(&vao);
        ebo.draw(RenderMode::triangles, index_count);
        PointLayout::bind(nullptr);
        IndexBuffer::bind(nullptr);
    }

}
