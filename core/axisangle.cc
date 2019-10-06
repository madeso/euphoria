#include "core/axisangle.h"

#include "core/assert.h"

namespace euphoria::core
{
    AxisAngle::AxisAngle(const unit3f& ax, const Angle& ang)
        : axis(ax), angle(ang)
    {
        ASSERT(ax.IsValid());
    }

    const AxisAngle
    AxisAngle::RightHandAround(const unit3f& axis, const Angle& angle)
    {
        ASSERT(axis.IsValid());
        return AxisAngle(axis, Angle::FromRadians(angle.InRadians()));
    }

    std::ostream&
    operator<<(std::ostream& stream, const AxisAngle& aa)
    {
        return stream << "(" << aa.axis << " " << aa.angle << ")";
    }
}  // namespace euphoria::core
