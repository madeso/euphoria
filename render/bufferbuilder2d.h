#ifndef RENDER_BUFFERBUILDER2D_H
#define RENDER_BUFFERBUILDER2D_H

#include <vector>
#include "core/vec2.h"

namespace euphoria::render
{
    struct Vao;
    struct Ebo;

    struct Point
    {
    public:
        Point(float x, float y, float u, float v);
        Point(const core::vec2f& apos, const core::vec2f& avert);

        const core::vec2f pos;

        // todo: rename to uv
        const core::vec2f vert;
    };

    struct BufferBuilder2d
    {
    public:
        BufferBuilder2d();

        void
        AddVertex(const Point& p);
        void
        AddTriangle(unsigned int a, unsigned int b, unsigned int c);
        void
        AddQuad(const Point& a, const Point& b, const Point& c, const Point& d);

        const std::vector<float>&
        GetVertexData() const;
        const std::vector<unsigned int>&
        GetTriangleIndices() const;

    private:
        std::vector<float>        data;
        std::vector<unsigned int> tris;
    };
}  // namespace euphoria::render

#endif  // RENDER_BUFFERBUILDER2D_H
