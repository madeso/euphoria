#pragma once

#include <iostream>

#include "core/vec3.h"
#include "core/angle.h"

namespace eu::core
{
    struct AxisAngle
    {
        /// a unit-vector
        unit3f axis;

        /// rotation according to right-hand rule
        core::Angle angle;

        [[nodiscard]] static AxisAngle
        from_right_hand_around(const unit3f& axis, const core::Angle& angle);

    private:
        AxisAngle(const unit3f& ax, const core::Angle& ang);
    };

    std::string to_string(const AxisAngle& aa);

}

ADD_DEFAULT_FORMATTER(eu::core::AxisAngle, std::string, eu::core::to_string);
