#ifndef EUPHORIA_POLARCOORD_H
#define EUPHORIA_POLARCOORD_H

#include "core/angle.h"
#include "core/vec3.h"

namespace euphoria::core
{
    class Random;

    // source: http://mathworld.wolfram.com/SphericalCoordinates.html
    class PolarCoord
    {
    public:
        PolarCoord(float azimuthal01, float polar01);

        unit3f
        ToCartesian() const;

        static PolarCoord
        Random(::euphoria::core::Random* random);

    private:
        Angle azimuthal_;  // longitude, 0-2pi
        Angle polar_;  // zenith, colatitude, 0-pi
    };

    unit3f
    RandomUnit3(Random* random);

}  // namespace euphoria::core

#endif  // EUPHORIA_POLARCOORD_H
