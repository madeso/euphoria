#pragma once

#include "core/angle.h"
#include "core/vec3.h"

namespace euphoria::core
{
    struct Random;

    // source: http://mathworld.wolfram.com/SphericalCoordinates.html
    struct PolarCoordinate
    {
        PolarCoordinate(float azimuthal01, float polar01);

        [[nodiscard]] Unit3f
        to_unit_vector() const;

        [[nodiscard]] static PolarCoordinate
        create_random(::euphoria::core::Random* random);

        Angle azimuthal; // longitude, 0-2pi
        Angle polar; // zenith, colatitude, 0-pi
    };

    Unit3f
    get_random_unit3(Random* random);

}
