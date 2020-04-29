#ifndef RENDER_BUFFERBUILDER2D_H
#define RENDER_BUFFERBUILDER2D_H

#include <vector>
#include "core/vec2.h"

namespace euphoria::core
{
    struct Point
    {
        Point(float x, float y, float u, float v);
        Point(const vec2f& p, const vec2f& u);

        const vec2f pos;
        const vec2f uv;
    };

    struct BufferBuilder2d
    {
        BufferBuilder2d();

        void
        AddVertex(const Point& p);

        // ccw
        void
        AddTriangle(unsigned int a, unsigned int b, unsigned int c);

        // bottom->top, left->right
        //  c --- d
        //  |     |
        //  a --- b
        void
        AddQuad(const Point& a, const Point& b, const Point& c, const Point& d);

        std::vector<float>        data;
        std::vector<unsigned int> tris;
    };
}  // namespace euphoria::core

#endif  // RENDER_BUFFERBUILDER2D_H
