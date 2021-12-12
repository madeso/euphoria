#include "core/polarcoord.h"

#include "core/numeric.h"
#include "core/range.h"
#include "core/assert.h"
#include "core/random.h"

namespace euphoria::core
{
    PolarCoordinate::PolarCoordinate(float azimuthal01, float polar01)
        : azimuthal(Angle::from_percent_of_360(azimuthal01))
        , polar(Angle::from_percent_of_180(polar01))
    {
        ASSERT(is_within(r01, azimuthal01));
        ASSERT(is_within(Range<float> {0, 2}, polar01));
    }

    Unit3f
    PolarCoordinate::to_unit_vector() const
    {
        const float cos_a = cos(azimuthal);
        const float sin_p = sin(polar);
        const float sin_a = sin(azimuthal);
        const float cos_p = cos(polar);

        const float x = cos_a * sin_p;
        const float y = sin_a * sin_p;
        const float z = cos_p;

        return Unit3f::to_unit(x, y, z);
    }

    PolarCoordinate
    PolarCoordinate::create_random(::euphoria::core::Random* random)
    {
        const float az = random->get_next_float01();
        const float polar = random->get_next_float01();
        return {az, polar};
    }

    Unit3f
    get_random_unit3(Random* random)
    {
        return PolarCoordinate::create_random(random).to_unit_vector();
    }

}
