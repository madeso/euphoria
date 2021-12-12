#pragma once

#include <string>

#include "core/rect.h"
#include "core/size2.h"
#include "core/noncopyable.h"

#include "gui/layoutdata.h"


namespace euphoria::render
{
    struct sprite_renderer;
}

namespace euphoria::gui
{
    struct ui_state;
    struct visitor;
}

namespace euphoria::gui
{
    struct lrtb
    {
        float left = 0.0f;
        float right = 0.0f;
        float top = 0.0f;
        float bottom = 0.0f;
    };


    struct widget
    {
        std::string name;
        lrtb margin;
        lrtb padding;

        explicit
        widget(gui::ui_state* state);

        virtual
        ~widget();

        NONCOPYABLE(widget);

        [[nodiscard]] bool
        is_active() const;

        [[nodiscard]] bool
        is_hot() const;

        virtual
        void
        step(float dt) = 0;

        virtual
        void
        on_size_changed();

        [[nodiscard]] core::Rectf
        get_client_rect() const;

        [[nodiscard]] core::Rectf
        get_background_rect() const;

        void
        set_rect(const core::Rectf& r);

        [[nodiscard]] core::size2f
        get_preferred_size() const;

        [[nodiscard]] virtual
        core::size2f
        calculate_minimum_size() const = 0;

        virtual
        void
        render(render::sprite_renderer* renderer) const = 0;

        virtual
        void
        visit(visitor* visitor) = 0;

        [[nodiscard]] const gui::ui_state&
        get_state() const;

        [[nodiscard]] gui::ui_state*
        get_state_ptr() const;

        gui::ui_state* ui_state;

        layout_data layout;
        core::Rectf rect;
    };
}
