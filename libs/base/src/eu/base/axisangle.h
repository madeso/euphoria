#pragma once

#include <iostream>

#include "eu/base/vec3.h"
#include "eu/base/angle.h"


namespace eu
{
    /** \addtogroup math
     *  @{
     */

    struct Q;

    /// Axis + Angle
    struct AA
    {
        /// a unit-vector
        n3 axis;

        /// rotation according to right-hand rule
        An angle;

        AA(const n3 &ax, const An &ang);

        [[nodiscard]] static std::optional<AA> from(const Q& q);
    };

    /// yaw + pitch + roll
    struct Ypr
    {
        /// left and right rotation, positive right
        An yaw = no_rotation;
        
        /// up and down rotation, positive up
        An pitch = no_rotation;
        
        /// tilting rotation, positive tilting right/clockwise/right hand rule
        An roll = no_rotation;
    };

    [[nodiscard]] AA
    rha(const n3& axis, const An& angle);

    std::string string_from(const AA &aa);
    std::string string_from(const Ypr& ypr);

    /** @}*/

    ADD_CATCH_FORMATTER_DEF(AA)
    ADD_CATCH_FORMATTER_DEF(Ypr)
}

ADD_DEFAULT_FORMATTER(eu::AA, std::string, eu::string_from);
ADD_DEFAULT_FORMATTER(eu::Ypr, std::string, eu::string_from);
