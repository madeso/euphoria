#pragma once

#include "render/buffer.h"

namespace euphoria::core
{
    struct buffer_builder2d;
}

namespace euphoria::render
{
    struct buffer2d
    {
    public:
        buffer2d(const core::buffer_builder2d& bb);

        void
        draw() const;

    private:
        int index_count;
        vertex_buffer vbo;
        point_layout vao;
        index_buffer ebo;
    };

}
