#pragma once

#include "base/vec2.h"

namespace eu::gui
{
    struct Widget;
}

namespace eu::gui
{
    struct State
    {
        State() = default;

        vec2f mouse = vec2f(0.0f, 0.0f);
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
