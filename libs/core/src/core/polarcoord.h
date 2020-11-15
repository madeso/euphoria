#ifndef EUPHORIA_POLARCOORD_H
#define EUPHORIA_POLARCOORD_H

#include "core/angle.h"
#include "core/vec3.h"

namespace euphoria::core
{
    struct Random;

    // source: http://mathworld.wolfram.com/SphericalCoordinates.html
    struct PolarCoord
    {
        PolarCoord(float azimuthal01, float polar01);

        unit3f
        ToUnitVector() const;

        [[nodiscard]] static PolarCoord
        Random(::euphoria::core::Random* random);

        Angle azimuthal;  // longitude, 0-2pi
        Angle polar;  // zenith, colatitude, 0-pi
    };

    unit3f
    RandomUnit3(Random* random);

}  // namespace euphoria::core

#endif  // EUPHORIA_POLARCOORD_H
