#include "render/buffer2d.h"

#include "core/assert.h"
#include "core/bufferbuilder2d.h"
#include "core/cint.h"

#include "render/shaderattribute2d.h"

namespace euphoria::render
{
    buffer2d::buffer2d(const core::buffer_builder2d& bb)
        : index_count(core::Csizet_to_int(bb.tris.size()))
    {
        point_layout::bind(&vao);
        vertex_buffer::bind(&vbo);

        vbo.set_data(bb.data);

        index_buffer::bind(&ebo);
        ebo.set_data(bb.tris);

        vao.bind_data(attributes2d::vertex(), sizeof(float) * 4, 0);

        point_layout::bind(nullptr);

        index_buffer::bind(nullptr);
        vertex_buffer::bind(nullptr);
    }


    void
    buffer2d::draw() const
    {
        index_buffer::bind(&ebo);
        point_layout::bind(&vao);
        ebo.draw(render_mode::triangles, index_count);
        point_layout::bind(nullptr);
        index_buffer::bind(nullptr);
    }

}
