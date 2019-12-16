#include "core/orbitcontroller.h"

namespace euphoria::core
{
    OrbitController::OrbitController()
        : center(vec3f::Zero())
        , distance(1.0f)
        , horizontal_rotation(Angle::FromDegrees(45.0f))
        , vertical_rotation(Angle::FromDegrees(45.0f))
    {}


    quatf
    OrbitController::GetRotation() const
    {
        return quatf::FromYawPitchRoll
        (
            horizontal_rotation,
            vertical_rotation,
            Angle::Zero()
        );
    }

    void
    OrbitController::Pan(const float dx, const float dy)
    {
        center += GetRotation().RightUpIn(vec3f{dx, dy, 0}) * 0.001f;
    }

    void
    OrbitController::Rotate(const float dx, const float dy)
    {
        horizontal_rotation += Angle::FromDegrees(dx);
        vertical_rotation += Angle::FromDegrees(dy);
    }


    vec3f
    OrbitController::GetCameraPosition()
    {
        return center - GetRotation().In() * distance;
    }
}
