#pragma once

#include "core/vec2.h"

namespace euphoria::gui
{
    struct Widget;
}

namespace euphoria::gui
{
    struct State
    {
        State() = default;

        core::Vec2f mouse = core::Vec2f(0.0f, 0.0f);
        bool mouse_down = false;

        Widget* hot = nullptr;
        Widget* active = nullptr;
        bool has_active = false;

        void
        begin();

        void
        end();

        void
        set_hot(Widget* w);

        void
        set_active(Widget* w);

        [[nodiscard]] bool
        is_mouse_down() const;
    };
}
