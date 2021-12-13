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
        Light();

        // todo(Gustav): move this to a light def file and let it be specified in a editor

        enum class type
        {
            directional,
            point,
            spot
        };

        type light_type;
        core::Vec3f position;
        core::Unit3f direction;
        core::Rgb ambient;
        core::Rgb diffuse;
        core::Rgb specular;
        core::Angle cutoff_angle_outer;
        core::Angle cutoff_angle_inner;

        LightAttenuation attenuation;
    };

}
