#pragma once

#include "core/noncopyable.h"

#include "gui/widget.h"
#include "gui/layoutcontainer.h"
#include "gui/visitor.h"


namespace euphoria::gui
{
    struct panel_widget : widget
    {
        panel_widget(gui::ui_state* state);
        ~panel_widget() override = default;

        NONCOPYABLE(panel_widget);

        void
        render(render::sprite_renderer* renderer) const override;

        void
        visit(visitor* visitor) override;

        void
        on_size_changed() override;

        [[nodiscard]] core::size2f
        calculate_minimum_size() const override;

        void
        step(float dt) override;

        layout_container container;
    };
}
