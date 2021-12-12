#pragma once

#include "core/vec2.h"

namespace euphoria::gui
{
    struct widget;
}

namespace euphoria::gui
{
    struct ui_state
    {
        ui_state() = default;

        core::Vec2f mouse = core::Vec2f(0.0f, 0.0f);
        bool mouse_down = false;

        widget* hot = nullptr;
        widget* active = nullptr;
        bool has_active = false;

        void
        begin();

        void
        end();

        void
        set_hot(widget* w);

        void
        set_active(widget* w);

        [[nodiscard]] bool
        is_mouse_down() const;
    };
}
