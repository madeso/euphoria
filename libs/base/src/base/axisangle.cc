#include "base/axisangle.h"

#include "assert/assert.h"

namespace eu
{
    AxisAngle::AxisAngle(const n3 &ax, const Angle &ang)
        : axis(ax), angle(ang)
    {
        ASSERT(ax.is_valid());
    }

    AxisAngle
    AxisAngle::from_right_hand_around(const n3 &axis, const Angle &angle)
    {
        ASSERT(axis.is_valid());
        return {axis, Angle::from_radians(angle.as_radians())};
    }

    std::string
    to_string(const AxisAngle &aa)
    {
        return fmt::format("({} {})", aa.axis, aa.angle);
    }
}
