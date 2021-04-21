#include "core/polarcoord.h"

#include "core/numeric.h"
#include "core/range.h"
#include "core/assert.h"
#include "core/random.h"

namespace euphoria::core
{
    polar_coord::polar_coord(float azimuthal01, float polar01)
        : azimuthal(angle::from_percent_of_360(azimuthal01))
        , polar(angle::from_percent_of_180(polar01))
    {
        ASSERT(IsWithin(R01(), azimuthal01));
        ASSERT(IsWithin(Range<float> {0, 2}, polar01));
    }

    unit3f
    polar_coord::to_unit_vector() const
    {
        const float cos_a = cos(azimuthal);
        const float sin_p = sin(polar);
        const float sin_a = sin(azimuthal);
        const float cos_p = cos(polar);

        const float x = cos_a * sin_p;
        const float y = sin_a * sin_p;
        const float z = cos_p;

        return unit3f::to_unit(x, y, z);
    }

    polar_coord
    polar_coord::create_random(::euphoria::core::Random* random)
    {
        const float az    = random->NextFloat01();
        const float polar = random->NextFloat01();
        return {az, polar};
    }

    unit3f
    create_random_unit3(Random* random)
    {
        return polar_coord::create_random(random).to_unit_vector();
    }

}  // namespace euphoria::core
