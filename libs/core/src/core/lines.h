#pragma once

#include "core/vec3.h"
#include "core/rgb.h"


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
        using FromToIndices = std::tuple<int, int>;
        std::vector<LinePoint> points;
        std::vector<FromToIndices> indices;

        void add_line(const Vec3f& from, const Vec3f& to, const Rgb& c);
    };
}
