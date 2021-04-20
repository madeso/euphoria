#include "core/fpscontroller.h"

namespace euphoria::core
{
    using namespace euphoria::convert;

    FpsController::FpsController()
        : rotation_(0.0_rad), look_(0.0_rad), position(vec3f::Zero())
    {}

    void
    FpsController::Look(float x, float y)
    {
        rotation_ += angle::from_degrees(-x * sensitivity);
        look_ += angle::from_degrees(-y * sensitivity);
    }

    void
    FpsController::MoveLeft(bool down)
    {
        left_down_ = down;
    }
    void
    FpsController::MoveRight(bool down)
    {
        right_down_ = down;
    }
    void
    FpsController::MoveForward(bool down)
    {
        forward_down_ = down;
    }
    void
    FpsController::MoveBackward(bool down)
    {
        backward_down_ = down;
    }
    void
    FpsController::MoveUp(bool down)
    {
        up_down_ = down;
    }
    void
    FpsController::MoveDown(bool down)
    {
        down_down_ = down;
    }

    void
    FpsController::HandleKey(Key key, bool down)
    {
        switch(key)
        {
        case Key::W:
        case Key::UP: MoveForward(down); return;
        case Key::S:
        case Key::DOWN: MoveBackward(down); return;
        case Key::A:
        case Key::LEFT: MoveLeft(down); return;
        case Key::D:
        case Key::RIGHT: MoveRight(down); return;

        case Key::SPACE: MoveUp(down); return;
        case Key::CTRL_LEFT: MoveDown(down); return;
        default: return;
        }
    }

    void
    FpsController::Update(float delta)
    {
        int forward = 0;
        int right   = 0;
        int up      = 0;

        if(forward_down_)
        {
            forward += 1;
        }
        if(backward_down_)
        {
            forward -= 1;
        }

        if(right_down_)
        {
            right += 1;
        }
        if(left_down_)
        {
            right -= 1;
        }

        if(up_down_)
        {
            up += 1;
        }
        if(down_down_)
        {
            up -= 1;
        }

        if(forward == 0 && right == 0 && up == 0)
        {
            return;
        }

        const auto input
                = GetRotation()
                          .RightUpIn(vec3f {static_cast<float>(right),
                                            static_cast<float>(up),
                                            static_cast<float>(forward)})
                          .GetNormalized();
        const auto movement = input * speed * delta;

        position += movement;
    }

    quatf
    FpsController::GetRotation() const
    {
        const auto rotation = quatf::FromAxisAngle(
                AxisAngle::RightHandAround(unit3f::YAxis(), rotation_));
        const auto look = quatf::FromAxisAngle(
                AxisAngle::RightHandAround(unit3f::XAxis(), look_));
        return rotation * look;
    }

}  // namespace euphoria::core
