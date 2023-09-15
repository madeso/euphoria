#include "core/lines.h"

#include "core/cint.h"

namespace eu::core
{
    LinePoint::LinePoint(const vec3f& p, const Rgb& c)
        : point(p)
        , color(c)
    {}


    void
    Lines::add_line(const vec3f& from, const vec3f& to, const Rgb& c)
    {
        const auto first_point_index = c_sizet_to_int(points.size());
        points.emplace_back(from, c);
        points.emplace_back(to, c);
        indices.emplace_back(first_point_index, first_point_index + 1);
    }
}
