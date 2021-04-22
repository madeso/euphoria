#pragma once

#include "render/buffer.h"

namespace euphoria::core
{
    struct buffer_builder2d;
}

namespace euphoria::render
{
    struct Buffer2d
    {
    public:
        Buffer2d(const core::buffer_builder2d& bb);

        void
        Draw() const;

    private:
        int index_count;
        VertexBuffer vbo;
        PointLayout  vao;
        IndexBuffer  ebo;
    };

}
