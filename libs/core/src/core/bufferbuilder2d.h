#pragma once




#include "base/vec2.h"


namespace eu::core
{
    struct BufferPoint2
    {
        const vec2f pos;
        const vec2f uv;

        BufferPoint2(float x, float y, float u, float v);
        BufferPoint2(const vec2f& p, const vec2f& u);
    };

    struct BufferBuilder2
    {
        std::vector<float> data;
        std::vector<unsigned int> tris;

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
    };
}
