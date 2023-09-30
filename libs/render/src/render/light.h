#pragma once

#include "base/vec3.h"
#include "core/rgb.h"
#include "base/angle.h"

namespace eu::render
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
        vec3f position;
        unit3f direction;
        core::Rgb ambient;
        core::Rgb diffuse;
        core::Rgb specular;
        Angle cutoff_angle_outer;
        Angle cutoff_angle_inner;

        LightAttenuation attenuation;

        Light();
    };

}
