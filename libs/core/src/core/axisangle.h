#pragma once

#include <iostream>

#include "core/vec3.h"
#include "core/angle.h"

namespace euphoria::core
{
    struct axis_angle
    {
        [[nodiscard]] static axis_angle
        right_hand_around(const unit3f& axis, const core::angle& angle);

        /** a unit-vector.
         */
        unit3f axis;

        /** rotation according to right-hand rule.
         */
        core::angle angle;

    private:
        axis_angle(const unit3f& ax, const core::angle& ang);
    };

    std::ostream&
    operator<<(std::ostream& stream, const axis_angle& aa);

}
