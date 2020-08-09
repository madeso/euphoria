#ifndef EUPHORIA_LIGHT_H
#define EUPHORIA_LIGHT_H

#include "core/vec3.h"
#include "core/rgb.h"
#include "core/angle.h"

namespace euphoria::render
{
    struct LightAttenuation
    {
    public:
        LightAttenuation();

        float
        GetConstant() const;

        float
        GetLinear() const;

        float
        GetQuadratic() const;

    private:
        float constant_;
        float linear_;
        float quadratic_;
    };

    struct Light
    {
    public:
        Light();

        // todo: move this to a light def file and let it be specified in a editor

        enum class Type : int
        {
            Directional,
            Point
        };
        
        Type         type;
        core::vec3f  position;
        core::unit3f direction;
        core::Rgb    ambient;
        core::Rgb    diffuse;
        core::Rgb    specular;
        core::Angle  cutoff_angle_outer;
        core::Angle  cutoff_angle_inner;

        LightAttenuation attenuation;
    };

}  // namespace euphoria::render

#endif  // EUPHORIA_LIGHT_H
