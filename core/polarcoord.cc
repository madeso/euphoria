#include "core/polarcoord.h"

#include "core/numeric.h"
#include "core/range.h"
#include "core/assert.h"
#include "core/random.h"

namespace euphoria::core
{
    PolarCoord::PolarCoord(float azimuthal01, float polar01)
        : azimuthal(Angle::FromPercentOf360(azimuthal01))
        , polar(Angle::FromPercentOf180(polar01))
    {
        ASSERT(IsWithin(R01(), azimuthal01));
        ASSERT(IsWithin(Range<float>{0,2}, polar01));
    }

    unit3f
    PolarCoord::ToUnitVector() const
    {
        const float cos_a = Cos(azimuthal);
        const float sin_p = Sin(polar);
        const float sin_a = Sin(azimuthal);
        const float cos_p = Cos(polar);

        const float x = cos_a * sin_p;
        const float y = sin_a * sin_p;
        const float z = cos_p;

        return unit3f::ToUnit(x, y, z);
    }

    PolarCoord
    PolarCoord::Random(::euphoria::core::Random* random)
    {
        const float az    = random->NextFloat01();
        const float polar = random->NextFloat01();
        return {az, polar};
    }

    unit3f
    RandomUnit3(Random* random)
    {
        return PolarCoord::Random(random).ToUnitVector();
    }

}  // namespace euphoria::core
