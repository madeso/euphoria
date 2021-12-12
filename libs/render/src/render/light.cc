#include "render/light.h"

namespace euphoria::render
{
    light::light()
        : light_type(type::directional)
        , position(core::Vec3f::zero())
        , direction(core::Vec3f {-0.2f, -1.0f, -0.3f}.get_normalized())
        , ambient(0.3f)
        , diffuse(core::NamedColor::white)
        , specular(core::NamedColor::white)
        , cutoff_angle_outer(core::Angle::from_degrees(18.0f))
        , cutoff_angle_inner(core::Angle::from_degrees(13.37f))
    {
    }

}
