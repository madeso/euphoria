#pragma once

#include <string>
#include <vector>

#include "core/vec2.h"


namespace euphoria::core
{
    struct point
    {
        point(float x, float y, float u, float v);
        point(const vec2f& p, const vec2f& u);

        const vec2f pos;
        const vec2f uv;
    };

    struct buffer_builder2d
    {
        buffer_builder2d();

        void
        add_vertex(const point& p);

        // ccw
        void
        add_triangle(unsigned int a, unsigned int b, unsigned int c);

        // bottom->top, left->right
        //  c --- d
        //  |     |
        //  a --- b
        void
        add_quad(const point& a, const point& b, const point& c, const point& d);

        void
        dump(const std::string& filename) const;

        std::vector<float> data;
        std::vector<unsigned int> tris;
    };
}
