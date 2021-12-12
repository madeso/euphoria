#include "core/orbitcontroller.h"

#include "core/range.h"

namespace euphoria::core
{
    OrbitController::OrbitController()
        : center(Vec3f::zero())
        , horizontal_rotation(Angle::from_degrees(45.0f))
        , vertical_rotation(Angle::from_degrees(45.0f))
    {}


    Quatf
    OrbitController::get_rotation() const
    {
        const auto hor =
            Quatf::from_axis_angle
            (
                AxisAngle::right_hand_around
                (
                    Unit3f::up(),
                    -horizontal_rotation
                )
            );
        const auto vert =
            Quatf::from_axis_angle
            (
                AxisAngle::right_hand_around
                (
                    Unit3f::right(),
                    -vertical_rotation
                )
            );
        return hor * vert;
    }


    void
    OrbitController::on_pan_input(float dx, float dy)
    {
        const auto movement = get_rotation().create_from_right_up_in(Vec3f
        {
            dx * pan_dx.get_multiplier_with_sign(),
            -dy * pan_dy.get_multiplier_with_sign(),
            0
        });
        center += movement * 0.01f;
    }


    void
    OrbitController::on_rotate_input(float dx, float dy)
    {
        horizontal_rotation += Angle::from_degrees
        (
            -dx * rotate_dx.get_multiplier_with_sign()
        );
        horizontal_rotation.wrap();

        vertical_rotation += Angle::from_degrees
        (
            -dy * rotate_dy.get_multiplier_with_sign()
        );

        const auto r = make_range(-Angle::quarter(), Angle::quarter());
        vertical_rotation = keep_within(r, vertical_rotation);
    }

    void
    OrbitController::on_zoom_input(float z)
    {
        distance = max(0.0f, distance + z * zoom.get_multiplier_with_sign());
    }


    Vec3f
    OrbitController::get_camera_position() const
    {
        return center - get_rotation().in() * distance;
    }
}
