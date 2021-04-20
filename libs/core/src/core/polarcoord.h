#ifndef EUPHORIA_POLARCOORD_H
#define EUPHORIA_POLARCOORD_H

#include "core/angle.h"
#include "core/vec3.h"

namespace euphoria::core
{
    struct Random;

    // source: http://mathworld.wolfram.com/SphericalCoordinates.html
    struct polar_coord
    {
        polar_coord(float azimuthal01, float polar01);

        [[nodiscard]] unit3f
        to_unit_vector() const;

        [[nodiscard]] static polar_coord
        create_random(::euphoria::core::Random* random);

        angle azimuthal;  // longitude, 0-2pi
        angle polar;  // zenith, colatitude, 0-pi
    };

    unit3f
    create_random_unit3(Random* random);

}  // namespace euphoria::core

#endif  // EUPHORIA_POLARCOORD_H
