#include "core/fpscontroller.h"

namespace euphoria::core
{
    using namespace euphoria::convert;

    FpsController::FpsController()
        : rotation_angle(0.0_rad)
        , look_angle(0.0_rad)
        , position(zero3f)
        , look_sensitivity(0.10f)
    {}

    void
    FpsController::look(float x, float y)
    {
        rotation_angle += angle::from_degrees(-x * look_sensitivity.get_multiplier_with_sign());
        look_angle += angle::from_degrees(-y * look_sensitivity.get_multiplier_with_sign());

        rotation_angle.wrap();
    }

    void
    FpsController::move_left(bool down)
    {
        is_left_down = down;
    }
    void
    FpsController::move_right(bool down)
    {
        is_right_down = down;
    }
    void
    FpsController::move_forward(bool down)
    {
        is_forward_down = down;
    }
    void
    FpsController::move_backward(bool down)
    {
        is_backward_down = down;
    }
    void
    FpsController::move_up(bool down)
    {
        is_up_down = down;
    }
    void
    FpsController::move_down(bool down)
    {
        is_down_down = down;
    }

    void
    FpsController::on_key(Key key, bool down)
    {
        switch(key)
        {
        case Key::w:
        case Key::up: move_forward(down); return;
        case Key::s:
        case Key::down: move_backward(down); return;
        case Key::a:
        case Key::left: move_left(down); return;
        case Key::d:
        case Key::right: move_right(down); return;

        case Key::space: move_up(down); return;
        case Key::ctrl_left: move_down(down); return;
        default: return;
        }
    }

    void
    FpsController::update(float delta)
    {
        int forward = 0;
        int right = 0;
        int up = 0;

        if(is_forward_down)
        {
            forward += 1;
        }
        if(is_backward_down)
        {
            forward -= 1;
        }

        if(is_right_down)
        {
            right += 1;
        }
        if(is_left_down)
        {
            right -= 1;
        }

        if(is_up_down)
        {
            up += 1;
        }
        if(is_down_down)
        {
            up -= 1;
        }

        if(forward == 0 && right == 0 && up == 0)
        {
            return;
        }

        const auto input = get_rotation()
            .create_from_right_up_in
            (
                vec3f
                {
                    static_cast<float>(right),
                    static_cast<float>(up),
                    static_cast<float>(forward)
                }
            )
            .get_normalized()
            ;
        const auto movement = input * move_speed * delta;

        position += movement;
    }

    quatf
    FpsController::calc_rotation(const angle& rotation_angle, const angle& look_angle)
    {
        const auto rotation = quatf::from_axis_angle
        (
            AxisAngle::from_right_hand_around(common::y_axis, rotation_angle)
        );
        const auto look = quatf::from_axis_angle
        (
            AxisAngle::from_right_hand_around(common::x_axis, look_angle)
        );
        return rotation * look;
    }

    quatf
    FpsController::get_rotation() const
    {
        return calc_rotation(rotation_angle, look_angle);
    }


    void
    FpsController::look_in_direction(const unit3f& direction)
    {
        look_angle = core::asin(direction.y);
        rotation_angle = core::atan2(direction.x, direction.z) + angle::from_degrees(180.0f);
    }

}
