#include "core/orbitcontroller.h"

#include "core/range.h"

namespace euphoria::core
{
    orbit_controller::orbit_controller()
        : center(vec3f::zero())
        , horizontal_rotation(angle::from_degrees(45.0f))
        , vertical_rotation(angle::from_degrees(45.0f))
    {}


    quatf
    orbit_controller::get_rotation() const
    {
        const auto hor =
            quatf::from_axis_angle
            (
                axis_angle::right_hand_around
                (
                    unit3f::up(),
                    -horizontal_rotation
                )
            );
        const auto vert =
            quatf::from_axis_angle
            (
                axis_angle::right_hand_around
                (
                    unit3f::right(),
                    -vertical_rotation
                )
            );
        return hor * vert;
    }


    void
    orbit_controller::on_pan_input(float dx, float dy)
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
    orbit_controller::on_rotate_input(float dx, float dy)
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

        const auto r = make_range(-angle::Quarter(), angle::Quarter());
        vertical_rotation = keep_within(r, vertical_rotation);
    }

    void
    orbit_controller::on_zoom_input(float z)
    {
        distance = max(0.0f, distance + z * zoom.get_multiplier_with_sign());
    }


    vec3f
    orbit_controller::get_camera_position() const
    {
        return center - get_rotation().in() * distance;
    }
}
