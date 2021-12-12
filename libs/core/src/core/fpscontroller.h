#pragma once

#include "core/vec3.h"
#include "core/angle.h"
#include "core/quat.h"
#include "core/key.h"

namespace euphoria::core
{
    struct FpsController
    {
        FpsController();

        void
        look(float delta_rot, float delta_look);

        void
        move_left(bool down);
        void
        move_right(bool down);
        void
        move_forward(bool down);
        void
        move_backward(bool down);
        void
        move_up(bool down);
        void
        move_down(bool down);

        void
        on_key(Key key, bool down);

        void
        update(float delta);

        [[nodiscard]] Quatf
        get_rotation() const;

        Angle rotation_angle;
        Angle look_angle;

        bool is_left_down = false;
        bool is_right_down = false;
        bool is_forward_down = false;
        bool is_backward_down = false;
        bool is_up_down = false;
        bool is_down_down = false;

        Vec3f position;
        float move_speed = 3.0f;
        float look_sensitivity = 0.10f;
    };

}

