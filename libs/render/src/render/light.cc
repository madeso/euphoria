#include "render/light.h"

namespace euphoria::render
{
    LightAttenuation::LightAttenuation()
        : constant_(1.0f), linear_(0.09f), quadratic_(0.032f)
    {}

    float
    LightAttenuation::GetConstant() const
    {
        return constant_;
    }

    float
    LightAttenuation::GetLinear() const
    {
        return linear_;
    }

    float
    LightAttenuation::GetQuadratic() const
    {
        return quadratic_;
    }

    ////////////////////////////////////////////////////////////////////////////////

    Light::Light()
        : type(Type::Directional)
        , position(core::vec3f::Zero())
        , direction(core::vec3f {-0.2f, -1.0f, -0.3f}.GetNormalized())
        , ambient(0.3f)
        , diffuse(core::Color::White)
        , specular(core::Color::White)
        , cutoff_angle_outer(core::Angle::FromDegrees(18.0f))
        , cutoff_angle_inner(core::Angle::FromDegrees(13.37f))
    {}

}
