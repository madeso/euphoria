#pragma once

#include "base/angle.h"
#include "base/vec3.h"

namespace eu
{
    struct Random;

    // source: http://mathworld.wolfram.com/SphericalCoordinates.html
    struct PolarCoordinate
    {
        Angle azimuthal; // longitude, 0-2pi
        Angle polar; // zenith, colatitude, 0-pi

        PolarCoordinate(float azimuthal01, float polar01);

        [[nodiscard]] static PolarCoordinate create_random(::eu::Random* random);

        [[nodiscard]] unit3f to_unit_vector() const;
    };

    unit3f get_random_unit3(Random* random);

}
