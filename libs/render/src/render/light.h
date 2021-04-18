#pragma once

#include "core/vec3.h"
#include "core/rgb.h"
#include "core/angle.h"

namespace euphoria::render
{
    struct LightAttenuation
    {
        LightAttenuation();

        [[nodiscard]] float
        GetConstant() const;

        [[nodiscard]] float
        GetLinear() const;

        [[nodiscard]] float
        GetQuadratic() const;

        float constant_;
        float linear_;
        float quadratic_;
    };


    struct Light
    {
        Light();

        // todo(Gustav): move this to a light def file and let it be specified in a editor

        enum class Type : int
        {
            Directional,
            Point,
            Spot
        };

        Type type;
        core::vec3f position;
        core::unit3f direction;
        core::Rgb ambient;
        core::Rgb diffuse;
        core::Rgb specular;
        core::Angle cutoff_angle_outer;
        core::Angle cutoff_angle_inner;

        LightAttenuation attenuation;
    };

}
