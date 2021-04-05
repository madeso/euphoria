#pragma once

#include "render/buffer.h"

namespace euphoria::core
{
    struct BufferBuilder2d;
}

namespace euphoria::render
{
    struct Buffer2d
    {
    public:
        Buffer2d(const core::BufferBuilder2d& bb);

        void
        Draw() const;

    private:
        int index_count;
        VertexBuffer vbo;
        PointLayout  vao;
        IndexBuffer  ebo;
    };

}
