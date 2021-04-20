#include "core/orbitcontroller.h"

#include "core/range.h"

namespace euphoria::core
{
    OrbitController::OrbitController()
        : center(vec3f::Zero())
        , horizontal_rotation(angle::from_degrees(45.0f))
        , vertical_rotation(angle::from_degrees(45.0f))
    {}


    quatf
    OrbitController::GetRotation() const
    {
        const auto hor =
            quatf::FromAxisAngle
            (
                AxisAngle::RightHandAround
                (
                    unit3f::Up(),
                    -horizontal_rotation
                )
            );
        const auto vert =
            quatf::FromAxisAngle
            (
                AxisAngle::RightHandAround
                (
                    unit3f::Right(),
                    -vertical_rotation
                )
            );
        return hor * vert;
    }


    void
    OrbitController::Pan(float dx, float dy)
    {
        const auto movement = GetRotation().RightUpIn(vec3f
        {
            dx * pan_dx.GetValueWithSign(),
            -dy * pan_dy.GetValueWithSign(),
            0
        });
        center += movement * 0.01f;
    }


    void
    OrbitController::Rotate(float dx, float dy)
    {
        horizontal_rotation += angle::from_degrees
        (
            -dx * rotate_dx.GetValueWithSign()
        );
        horizontal_rotation.wrap();

        vertical_rotation += angle::from_degrees
        (
            -dy * rotate_dy.GetValueWithSign()
        );

        const auto r = MakeRange(-angle::Quarter(), angle::Quarter());
        vertical_rotation = KeepWithin(r, vertical_rotation);
    }

    void
    OrbitController::Zoom(float z)
    {
        distance = Max(0.0f, distance + z * zoom.GetValueWithSign());
    }


    vec3f
    OrbitController::GetCameraPosition() const
    {
        return center - GetRotation().In() * distance;
    }
}
