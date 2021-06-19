#include "core/lines.h"

#include "core/cint.h"

namespace euphoria::core
{
    line_point::line_point(const vec3f& p, const rgb& c)
        : point(p)
        , color(c)
    {}


    void
    lines::add_line(const vec3f& from, const vec3f& to, const rgb& c)
    {
        const auto first_point_index = c_sizet_to_int(points.size());
        points.emplace_back(from, c);
        points.emplace_back(to, c);
        indices.emplace_back(first_point_index, first_point_index + 1);
    }
}
