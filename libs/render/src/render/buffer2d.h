#pragma once

#include "render/buffer.h"

namespace eu::core
{
    struct BufferBuilder2;
}

namespace eu::render
{
    struct Buffer2d
    {
    public:
        Buffer2d(const core::BufferBuilder2& bb);

        void
        draw() const;

    private:
        int index_count;
        VertexBuffer vbo;
        PointLayout vao;
        IndexBuffer ebo;
    };

}
