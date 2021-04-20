#ifndef CORE_LINES_H
#define CORE_LINES_H

#include "core/vec3.h"
#include "core/rgb.h"

#include <vector>
#include <tuple>

namespace euphoria::core
{
    struct line_point
    {
        line_point(const vec3f& p, const rgb& c);

        vec3f point;
        rgb color;
    };

    using line = std::tuple<int, int>;

    struct lines
    {
        std::vector<line_point> points;
        std::vector<line> indices;

        void add_line(const vec3f& from, const vec3f& to, const rgb& c);
    };
}

#endif  // CORE_LINES_H
