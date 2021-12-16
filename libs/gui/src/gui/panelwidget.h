#pragma once

#include "core/noncopyable.h"

#include "gui/widget.h"
#include "gui/layoutcontainer.h"
#include "gui/visitor.h"


namespace euphoria::gui
{
    struct PanelWidget : Widget
    {
        PanelWidget(gui::State* state);
        ~PanelWidget() override = default;

        NONCOPYABLE(PanelWidget);

        void
        render(render::SpriteRenderer* renderer) const override;

        void
        visit(Visitor* visitor) override;

        void
        on_size_changed() override;

        [[nodiscard]] core::Size2f
        calculate_minimum_size() const override;

        void
        step(float dt) override;

        LayoutContainer container;
    };
}
