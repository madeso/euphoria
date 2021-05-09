#pragma once

#include "core/angle.h"
#include "core/vec3.h"

namespace euphoria::core
{
    struct random;

    // source: http://mathworld.wolfram.com/SphericalCoordinates.html
    struct polar_coord
    {
        polar_coord(float azimuthal01, float polar01);

        [[nodiscard]] unit3f
        to_unit_vector() const;

        [[nodiscard]] static polar_coord
        create_random(::euphoria::core::random* random);

        angle azimuthal;  // longitude, 0-2pi
        angle polar;  // zenith, colatitude, 0-pi
    };

    unit3f
    get_random_unit3(random* random);

}
