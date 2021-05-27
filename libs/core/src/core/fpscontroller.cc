#include "core/fpscontroller.h"

namespace euphoria::core
{
    using namespace euphoria::convert;

    fps_controller::fps_controller()
        : rotation_angle(0.0_rad), look_angle(0.0_rad), position(vec3f::zero())
    {}

    void
    fps_controller::look(float x, float y)
    {
        rotation_angle += angle::from_degrees(-x * look_sensitivity);
        look_angle += angle::from_degrees(-y * look_sensitivity);
    }

    void
    fps_controller::move_left(bool down)
    {
        is_left_down = down;
    }
    void
    fps_controller::move_right(bool down)
    {
        is_right_down = down;
    }
    void
    fps_controller::move_forward(bool down)
    {
        is_forward_down = down;
    }
    void
    fps_controller::move_backward(bool down)
    {
        is_backward_down = down;
    }
    void
    fps_controller::move_up(bool down)
    {
        is_up_down = down;
    }
    void
    fps_controller::move_down(bool down)
    {
        is_down_down = down;
    }

    void
    fps_controller::on_key(key key, bool down)
    {
        switch(key)
        {
        case key::w:
        case key::up: move_forward(down); return;
        case key::s:
        case key::down: move_backward(down); return;
        case key::a:
        case key::left: move_left(down); return;
        case key::d:
        case key::right: move_right(down); return;

        case key::space: move_up(down); return;
        case key::ctrl_left: move_down(down); return;
        default: return;
        }
    }

    void
    fps_controller::update(float delta)
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
                          .create_from_right_up_in(vec3f {static_cast<float>(right),
                                            static_cast<float>(up),
                                            static_cast<float>(forward)})
                          .get_normalized();
        const auto movement = input * move_speed * delta;

        position += movement;
    }

    quatf
    fps_controller::get_rotation() const
    {
        const auto rotation = quatf::from_axis_angle(
                axis_angle::right_hand_around(unit3f::y_axis(), rotation_angle));
        const auto look = quatf::from_axis_angle(
                axis_angle::right_hand_around(unit3f::x_axis(), look_angle));
        return rotation * look;
    }

} // namespace euphoria::core
