#pragma once

#include <iostream>

#include "core/vec3.h"
#include "core/angle.h"

namespace euphoria::core
{
    struct AxisAngle
    {
        [[nodiscard]] static AxisAngle
        right_hand_around(const Unit3f& axis, const core::Angle& angle);

        /** a unit-vector.
         */
        Unit3f axis;

        /** rotation according to right-hand rule.
         */
        core::Angle angle;

    private:
        AxisAngle(const Unit3f& ax, const core::Angle& ang);
    };

    std::ostream&
    operator<<(std::ostream& stream, const AxisAngle& aa);

}
