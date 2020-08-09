#ifndef CORE_LINES_H
#define CORE_LINES_H

#include "core/vec3.h"
#include "core/rgb.h"

#include <vector>
#include <tuple>

namespace euphoria::core
{
    struct LinePoint
    {
        LinePoint(const vec3f& p, const Rgb& c);

        vec3f point;
        Rgb color;
    };

    using Line = std::tuple<int, int>;

    struct Lines
    {
        std::vector<LinePoint> points;
        std::vector<Line> lines;

        void AddLine(const vec3f& from, const vec3f& to, const Rgb& c);
    };
}

#endif  // CORE_LINES_H
