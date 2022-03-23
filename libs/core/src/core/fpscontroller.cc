#include "core/fpscontroller.h"

namespace euphoria::core
{
    using namespace euphoria::convert;

    FpsController::FpsController()
        : rotation_angle(0.0_rad)
        , look_angle(0.0_rad)
        , position(Vec3f::zero())
        , look_sensitivity(0.10f)
    {}

    void
    FpsController::look(float x, float y)
    {
        rotation_angle += Angle::from_degrees(-x * look_sensitivity.get_multiplier_with_sign());
        look_angle += Angle::from_degrees(-y * look_sensitivity.get_multiplier_with_sign());
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

        const auto input
                = get_rotation()
                          .create_from_right_up_in(Vec3f {static_cast<float>(right),
                                            static_cast<float>(up),
                                            static_cast<float>(forward)})
                          .get_normalized();
        const auto movement = input * move_speed * delta;

        position += movement;
    }

    Quatf
    FpsController::get_rotation() const
    {
        const auto rotation = Quatf::from_axis_angle(
                AxisAngle::right_hand_around(Unit3f::y_axis(), rotation_angle));
        const auto look = Quatf::from_axis_angle(
                AxisAngle::right_hand_around(Unit3f::x_axis(), look_angle));
        return rotation * look;
    }

}
