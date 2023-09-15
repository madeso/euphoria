#pragma once

#include "core/vec3.h"
#include "core/rgb.h"


#include <tuple>

namespace eu::core
{
    struct LinePoint
    {
        vec3f point;
        Rgb color;

        LinePoint(const vec3f& p, const Rgb& c);
    };


    struct Lines
    {
        using FromToIndices = std::tuple<int, int>;

        std::vector<LinePoint> points;
        std::vector<FromToIndices> indices;

        void add_line(const vec3f& from, const vec3f& to, const Rgb& c);
    };
}
