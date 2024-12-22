#pragma once

#include <iostream>

#include "base/vec3.h"
#include "base/angle.h"

namespace eu
{
    struct AA
    {
        /// a unit-vector
        n3 axis;

        /// rotation according to right-hand rule
        An angle;

        [[nodiscard]] static AA
        from_right_hand_around(const n3 &axis, const An &angle);

    private:
        AA(const n3 &ax, const An &ang);
    };

    std::string to_string(const AA &aa);

}

ADD_DEFAULT_FORMATTER(eu::AA, std::string, eu::to_string);
