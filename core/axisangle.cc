#include "core/axisangle.h"

#include "core/assert.h"

AxisAngle::AxisAngle(const vec3f::Unit& ax, const Angle& ang)
    : axis(ax.GetNormalized())
    , angle(ang)
{
  Assert(ax.IsValid());
}

const AxisAngle AxisAngle::RightHandAround(const vec3f::Unit& axis, const Angle& angle)
{
  Assert(axis.IsValid());
  return AxisAngle(axis, Angle::FromRadians(angle.inRadians()));
}

std::ostream& operator<<(std::ostream& stream, const AxisAngle& aa)
{
  return stream << "(" << aa.axis << " " << aa.angle << ")";
}
