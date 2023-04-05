#include "core/axisangle.h"

#include "assert/assert.h"

namespace euphoria::core
{
    AxisAngle::AxisAngle(const unit3f& ax, const core::angle& ang)
        : axis(ax), angle(ang)
    {
        ASSERT(ax.is_valid());
    }

    AxisAngle
    AxisAngle::from_right_hand_around(const unit3f& axis, const core::angle& angle)
    {
        ASSERT(axis.is_valid());
        return {axis, angle::from_radians(angle.as_radians())};
    }

    std::string
    to_string(const AxisAngle& aa)
    {
        return fmt::format("({} {})", aa.axis, aa.angle);
    }
}
