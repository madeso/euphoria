#pragma once

#include "core/vec3.h"
#include "core/rgb.h"
#include "core/angle.h"

namespace euphoria::render
{
    struct light_attenuation
    {
        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
    };


    struct light
    {
        light();

        // todo(Gustav): move this to a light def file and let it be specified in a editor

        enum class type
        {
            directional,
            point,
            spot
        };

        type light_type;
        core::vec3f position;
        core::unit3f direction;
        core::rgb ambient;
        core::rgb diffuse;
        core::rgb specular;
        core::angle cutoff_angle_outer;
        core::angle cutoff_angle_inner;

        light_attenuation attenuation;
    };

}
