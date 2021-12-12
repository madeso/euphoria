#pragma once

#include <string>
#include <vector>

#include "core/vec2.h"


namespace euphoria::core
{
    struct BufferPoint2
    {
        BufferPoint2(float x, float y, float u, float v);
        BufferPoint2(const Vec2f& p, const Vec2f& u);

        const Vec2f pos;
        const Vec2f uv;
    };

    struct BufferBuilder2
    {
        BufferBuilder2();

        void
        add_vertex(const BufferPoint2& p);

        // ccw
        void
        add_triangle(unsigned int a, unsigned int b, unsigned int c);

        // bottom->top, left->right
        //  c --- d
        //  |     |
        //  a --- b
        void
        add_quad(const BufferPoint2& a, const BufferPoint2& b, const BufferPoint2& c, const BufferPoint2& d);

        void
        dump(const std::string& filename) const;

        std::vector<float> data;
        std::vector<unsigned int> tris;
    };
}
