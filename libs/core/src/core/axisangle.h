#pragma once

#include <iostream>

#include "core/vec3.h"
#include "core/angle.h"

namespace euphoria::core
{
    struct AxisAngle
    {
        /// a unit-vector
        unit3f axis;

        /// rotation according to right-hand rule
        core::angle angle;

        [[nodiscard]] static AxisAngle
        from_right_hand_around(const unit3f& axis, const core::angle& angle);

    private:
        AxisAngle(const unit3f& ax, const core::angle& ang);
    };

    std::string to_string(const AxisAngle& aa);

}

ADD_DEFAULT_FORMATTER(euphoria::core::AxisAngle, std::string, euphoria::core::to_string);
