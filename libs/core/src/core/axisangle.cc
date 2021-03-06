#include "core/axisangle.h"

#include "core/assert.h"

namespace euphoria::core
{
    axis_angle::axis_angle(const unit3f& ax, const core::angle& ang)
        : axis(ax), angle(ang)
    {
        ASSERT(ax.is_valid());
    }

    axis_angle
    axis_angle::right_hand_around(const unit3f& axis, const core::angle& angle)
    {
        ASSERT(axis.is_valid());
        return axis_angle(axis, angle::from_radians(angle.in_radians()));
    }

    std::ostream&
    operator<<(std::ostream& stream, const axis_angle& aa)
    {
        return stream << "(" << aa.axis << " " << aa.angle << ")";
    }
}
