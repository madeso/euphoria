#pragma once

#include "gui/widget.h"
#include "gui/layoutcontainer.h"
#include "gui/visitor.h"


namespace euphoria::gui
{
    struct PanelWidget : Widget
    {
        LayoutContainer container;

        PanelWidget(gui::State* state);
        ~PanelWidget() override = default;

        PanelWidget(const PanelWidget&) = delete;
        PanelWidget(PanelWidget&&) = delete;
        void operator=(const PanelWidget&) = delete;
        void operator=(PanelWidget&&) = delete;

        void render(render::SpriteRenderer* renderer) const override;
        void visit(Visitor* visitor) override;
        void on_size_changed() override;
        [[nodiscard]] core::size2f calculate_minimum_size() const override;
        void step(float dt) override;
    };
}
