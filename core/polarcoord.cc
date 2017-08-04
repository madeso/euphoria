#include "core/polarcoord.h"

#include "core/numeric.h"
#include "core/assert.h"

PolarCoord::PolarCoord(Angle azimuthal, Angle polar)
  : azimuthal_(azimuthal)
  , polar_(polar)
{
  Assert(IsWithinInclusive(0, azimuthal.inDegrees(), 360));
  Assert(IsWithinInclusive(0, polar.inDegrees(), 180));
}

vec3f::Unit PolarCoord::ToCartesian() const
{
  const float cos_a = Cos(azimuthal_);
  const float sin_p = Sin(polar_);
  const float sin_a = Sin(azimuthal_);
  const float cos_p = Cos(polar_);

  const float x = cos_a * sin_p;
  const float y = sin_a * sin_p;
  const float z = cos_p;

  return vec3f::ToUnit(x,y,z);
}
