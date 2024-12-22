#pragma once

#include <iostream>

#include "base/vec3.h"
#include "base/angle.h"

namespace eu
{
    /// Axis + Angle
    struct AA
    {
        /// a unit-vector
        n3 axis;

        /// rotation according to right-hand rule
        An angle;

        AA(const n3 &ax, const An &ang);
    };

    /// yaw + pitch + roll
    struct Ypr
    {
        An yaw = no_rotation;
        An pitch = no_rotation;
        An roll = no_rotation;
    };

    [[nodiscard]] AA
    right_hand_around(const n3& axis, const An& angle);

    std::string string_from(const AA &aa);
    std::string string_from(const Ypr& ypr);

}

ADD_DEFAULT_FORMATTER(eu::AA, std::string, eu::string_from);
ADD_DEFAULT_FORMATTER(eu::Ypr, std::string, eu::string_from);
