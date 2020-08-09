#include "core/orbitcontroller.h"

#include "core/range.h"

namespace euphoria::core
{
    OrbitController::OrbitController()
        : center(vec3f::Zero())
        , horizontal_rotation(Angle::FromDegrees(45.0f))
        , vertical_rotation(Angle::FromDegrees(45.0f))
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
    OrbitController::Pan(const float dx, const float dy)
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
    OrbitController::Rotate(const float dx, const float dy)
    {
        horizontal_rotation += Angle::FromDegrees
        (
            -dx * rotate_dx.GetValueWithSign()
        );
        horizontal_rotation.Wrap();
        
        vertical_rotation += Angle::FromDegrees
        (
            -dy * rotate_dy.GetValueWithSign()
        );
        
        const auto r = MakeRange(-Angle::Quarter(), Angle::Quarter());
        vertical_rotation = KeepWithin(r, vertical_rotation);
    }

    void
    OrbitController::Zoom(const float z)
    {
        distance = Max(0.0f, distance + z * zoom.GetValueWithSign());
    }


    vec3f
    OrbitController::GetCameraPosition()
    {
        return center - GetRotation().In() * distance;
    }
}
