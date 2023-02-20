#pragma once

#include "core/vec3.h"
#include "core/rgb.h"
#include "core/angle.h"

namespace euphoria::render
{
    struct LightAttenuation
    {
        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
    };


    struct Light
    {
        enum class Type
        {
            directional,
            point,
            spot
        };

        // todo(Gustav): move this to a light def file and let it be specified in a editor

        Type light_type;
        core::vec3f position;
        core::unit3f direction;
        core::rgb ambient;
        core::rgb diffuse;
        core::rgb specular;
        core::angle cutoff_angle_outer;
        core::angle cutoff_angle_inner;

        LightAttenuation attenuation;

        Light();
    };

}
