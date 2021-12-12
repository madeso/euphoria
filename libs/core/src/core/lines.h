#pragma once

#include "core/vec3.h"
#include "core/rgb.h"

#include <vector>
#include <tuple>

namespace euphoria::core
{
    struct LinePoint
    {
        LinePoint(const Vec3f& p, const Rgb& c);

        Vec3f point;
        Rgb color;
    };


    struct Lines
    {
        using line = std::tuple<int, int>;
        std::vector<LinePoint> points;
        std::vector<line> indices;

        void add_line(const Vec3f& from, const Vec3f& to, const Rgb& c);
    };
}
