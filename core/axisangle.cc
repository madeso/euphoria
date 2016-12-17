#include "core/axisangle.h"

AxisAngle::AxisAngle(const vec3f& ax, const Angle& ang)
    : axis(ax.GetNormalized())
    , angle(ang)
{
}

const AxisAngle AxisAngle::RightHandAround(const vec3f& axis, const Angle& angle)
{
  return AxisAngle(axis, Angle::FromRadians(angle.inRadians()));
}
