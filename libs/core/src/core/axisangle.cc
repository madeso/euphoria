#include "core/axisangle.h"

#include "core/assert.h"

namespace euphoria::core
{
    AxisAngle::AxisAngle(const unit3f& ax, const core::angle& ang)
        : axis(ax), angle(ang)
    {
        ASSERT(ax.IsValid());
    }

    AxisAngle
    AxisAngle::RightHandAround(const unit3f& axis, const core::angle& angle)
    {
        ASSERT(axis.IsValid());
        return AxisAngle(axis, angle::from_radians(angle.in_radians()));
    }

    std::ostream&
    operator<<(std::ostream& stream, const AxisAngle& aa)
    {
        return stream << "(" << aa.axis << " " << aa.angle << ")";
    }
}  // namespace euphoria::core
