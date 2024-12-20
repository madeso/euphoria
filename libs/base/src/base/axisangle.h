#pragma once

#include <iostream>

#include "base/vec3.h"
#include "base/angle.h"

namespace eu
{
    struct AxisAngle
    {
        /// a unit-vector
        n3 axis;

        /// rotation according to right-hand rule
        Angle angle;

        [[nodiscard]] static AxisAngle
        from_right_hand_around(const n3 &axis, const Angle &angle);

    private:
        AxisAngle(const n3 &ax, const Angle &ang);
    };

    std::string to_string(const AxisAngle &aa);

}

ADD_DEFAULT_FORMATTER(eu::AxisAngle, std::string, eu::to_string);
