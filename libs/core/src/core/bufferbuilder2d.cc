#include "core/bufferbuilder2d.h"

#include <fstream>

#include "core/vec3.h"
#include "core/cint.h"

namespace euphoria::core
{
    BufferPoint2::BufferPoint2(float x, float y, float u, float v)
        : pos(x, y)
        , uv(u, v)
    {
    }


    BufferPoint2::BufferPoint2(const vec2f& p, const vec2f& u)
        : pos(p)
        , uv(u)
    {
    }


    BufferBuilder2::BufferBuilder2() = default;


    void
    BufferBuilder2::add_vertex(const BufferPoint2& p)
    {
        data.push_back(p.pos.x);
        data.push_back(p.pos.y);
        data.push_back(p.uv.x);
        data.push_back(p.uv.y);
    }


    namespace
    {
        bool
        is_ccw
        (
            const std::vector<float>& data,
            unsigned int a,
            unsigned int b,
            unsigned int c
        )
        {
            const auto va = vec3f{data[a*4], data[a*4+1], 0};
            const auto vb = vec3f{data[b*4], data[b*4+1], 0};
            const auto vc = vec3f{data[c*4], data[c*4+1], 0};
            const auto cr = cross
            (
                vec3f::from_to(va, vb),
                vec3f::from_to(va, vc)
            );
            return cr.z < 0;
        }
    }


    void
    BufferBuilder2::add_triangle
    (
        unsigned int a,
        unsigned int b,
        unsigned int c
    )
    {
        ASSERTX
        (
            a < (data.size() / 4) &&
            b < (data.size() / 4) &&
            c < (data.size() / 4),
            a, b, c, data.size()/4
        );
        ASSERTX(is_ccw(data, a, b, c), a, b, c);
        tris.push_back(a);
        tris.push_back(b);
        tris.push_back(c);
    }


    void
    BufferBuilder2::add_quad
    (
        const BufferPoint2& a,
        const BufferPoint2& b,
        const BufferPoint2& c,
        const BufferPoint2& d
    )
    {
        const unsigned int ai = c_sizet_to_int(data.size());
        const unsigned int bi = ai + 1;
        const unsigned int ci = ai + 2;
        const unsigned int di = ai + 3;

        add_vertex(a);
        add_vertex(b);
        add_vertex(c);
        add_vertex(d);

        add_triangle(ci, bi, ai);
        add_triangle(ci, di, bi);
    }


    void
    BufferBuilder2::dump(const std::string& filename) const
    {
        std::ofstream f(filename.c_str());

        for(size_t i=0; i<data.size(); i+=4)
        {
            f << "v " << data[i+0] << " " << data[i+1] << "\n";
        }

        for(size_t i=0; i<data.size(); i+=4)
        {
            f << "vt " << data[i+2] << " " << data[i+3] << "\n";
        }

        for(size_t i=0; i<tris.size(); i+=3)
        {
            const auto a = tris[i+0];
            const auto b = tris[i+1];
            const auto c = tris[i+2];
            f << "f "
                << a << "/" << a << " "
                << b << "/" << b << " "
                << c << "/" << c << " "
                << "\n";
        }
    }
}

