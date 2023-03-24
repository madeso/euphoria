#include "render/light.h"

namespace euphoria::render
{
    Light::Light()
        : light_type(Type::directional)
        , position(core::zero3f)
        , direction(core::vec3f {-0.2f, -1.0f, -0.3f}.get_normalized())
        , ambient(0.3f)
        , diffuse(core::NamedColor::white)
        , specular(core::NamedColor::white)
        , cutoff_angle_outer(core::angle::from_degrees(18.0f))
        , cutoff_angle_inner(core::angle::from_degrees(13.37f))
    {
    }

}
