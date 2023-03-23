#include "core/orbitcontroller.h"

#include "core/range.h"

namespace euphoria::core
{
    OrbitController::OrbitController()
        : center(vec3f::zero())
        , horizontal_rotation(angle::from_degrees(45.0f))
        , vertical_rotation(angle::from_degrees(45.0f))
    {}


    quatf
    OrbitController::get_rotation() const
    {
        const auto hor =
            quatf::from_axis_angle
            (
                AxisAngle::from_right_hand_around
                (
                    common::up,
                    -horizontal_rotation
                )
            );
        const auto vert =
            quatf::from_axis_angle
            (
                AxisAngle::from_right_hand_around
                (
                    common::right,
                    -vertical_rotation
                )
            );
        return hor * vert;
    }


    void
    OrbitController::on_pan_input(float dx, float dy)
    {
        const auto movement = get_rotation().create_from_right_up_in(vec3f
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
        horizontal_rotation += angle::from_degrees
        (
            -dx * rotate_dx.get_multiplier_with_sign()
        );
        horizontal_rotation.wrap();

        vertical_rotation += angle::from_degrees
        (
            -dy * rotate_dy.get_multiplier_with_sign()
        );

        const auto r = make_range(-quarter_turn, quarter_turn);
        vertical_rotation = keep_within(r, vertical_rotation);
    }

    void
    OrbitController::on_zoom_input(float z)
    {
        distance = max(0.0f, distance + z * zoom.get_multiplier_with_sign());
    }


    vec3f
    OrbitController::get_camera_position() const
    {
        return center - get_rotation().in() * distance;
    }
}
