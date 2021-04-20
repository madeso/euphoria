#ifndef CORE_AXISANGLE_H
#define CORE_AXISANGLE_H

#include <iostream>

#include "core/vec3.h"
#include "core/angle.h"

namespace euphoria::core
{
    struct AxisAngle
    {
        [[nodiscard]] static AxisAngle
        RightHandAround(const unit3f& axis, const core::angle& angle);

        /** a unit-vector.
         */
        unit3f axis;

        /** rotation according to right-hand rule.
         */
        angle angle;

    private:
        AxisAngle(const unit3f& ax, const core::angle& ang);
    };

    std::ostream&
    operator<<(std::ostream& stream, const AxisAngle& aa);

}  // namespace euphoria::core

#endif  // CORE_AXISANGLE_H
