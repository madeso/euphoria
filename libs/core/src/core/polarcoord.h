#pragma once

#include "core/angle.h"
#include "core/vec3.h"

namespace euphoria::core
{
    struct Random;

    // source: http://mathworld.wolfram.com/SphericalCoordinates.html
    struct PolarCoordinate
    {
        angle azimuthal; // longitude, 0-2pi
        angle polar; // zenith, colatitude, 0-pi

        PolarCoordinate(float azimuthal01, float polar01);

        [[nodiscard]] static PolarCoordinate create_random(::euphoria::core::Random* random);

        [[nodiscard]] unit3f to_unit_vector() const;
    };

    unit3f get_random_unit3(Random* random);

}
