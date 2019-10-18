#ifndef RENDER_BUFFER_2D_H
#define RENDER_BUFFER_2D_H

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
        unsigned int index_count_;
        VertexBuffer          vbo_;
        Vao          vao_;
        Ebo          ebo_;
    };

}  // namespace euphoria::render

#endif  // RENDER_BUFFER_2D_H
