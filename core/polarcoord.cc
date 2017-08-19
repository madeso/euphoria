#include "core/polarcoord.h"

#include "core/numeric.h"
#include "core/assert.h"

PolarCoord::PolarCoord(float azimuthal01, float polar01)
    : azimuthal_(Angle::FromPercentOf360(azimuthal01))
    , polar_(Angle::FromPercentOf180(polar01))
{
  ASSERT(IsWithinInclusive(0, azimuthal01, 1.0f));
  ASSERT(IsWithinInclusive(0, polar01, 2.0f));
}

vec3f::Unit
PolarCoord::ToCartesian() const
{
  const float cos_a = Cos(azimuthal_);
  const float sin_p = Sin(polar_);
  const float sin_a = Sin(azimuthal_);
  const float cos_p = Cos(polar_);

  const float x = cos_a * sin_p;
  const float y = sin_a * sin_p;
  const float z = cos_p;

  return vec3f::ToUnit(x, y, z);
}
