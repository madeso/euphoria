#include "core/polarcoord.h"

#include "core/numeric.h"
#include "core/range.h"
#include "core/assert.h"

PolarCoord::PolarCoord(float azimuthal01, float polar01)
    : azimuthal_(Angle::FromPercentOf360(azimuthal01))
    , polar_(Angle::FromPercentOf180(polar01))
{
  ASSERT(Range(0, 1).IsWithin(azimuthal01));
  ASSERT(Range(0, 2).IsWithin(polar01));
}

unit3f
PolarCoord::ToCartesian() const
{
  const float cos_a = Cos(azimuthal_);
  const float sin_p = Sin(polar_);
  const float sin_a = Sin(azimuthal_);
  const float cos_p = Cos(polar_);

  const float x = cos_a * sin_p;
  const float y = sin_a * sin_p;
  const float z = cos_p;

  return unit3f::ToUnit(x, y, z);
}
