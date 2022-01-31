#include "core/axisangle.h"

#include "assert/assert.h"

namespace euphoria::core
{
    AxisAngle::AxisAngle(const Unit3f& ax, const core::Angle& ang)
        : axis(ax), angle(ang)
    {
        ASSERT(ax.is_valid());
    }

    AxisAngle
    AxisAngle::right_hand_around(const Unit3f& axis, const core::Angle& angle)
    {
        ASSERT(axis.is_valid());
        return AxisAngle(axis, Angle::from_radians(angle.in_radians()));
    }

    std::ostream&
    operator<<(std::ostream& stream, const AxisAngle& aa)
    {
        return stream << "(" << aa.axis << " " << aa.angle << ")";
    }
}
