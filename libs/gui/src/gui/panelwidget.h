#pragma once

#include "core/noncopyable.h"

#include "gui/widget.h"
#include "gui/layoutcontainer.h"
#include "gui/visitor.h"


namespace euphoria::gui
{
    struct PanelWidget : public Widget
    {
        PanelWidget(UiState* state);
        ~PanelWidget() override = default;

        NONCOPYABLE(PanelWidget);

        void
        Render(render::sprite_renderer* renderer) const override;

        void
        Visit(Visitor* visitor) override;

        void
        OnSize() override;

        [[nodiscard]] core::Sizef
        CalculateMinimumSize() const override;

        void
        Step(float dt) override;

        LayoutContainer container;
    };
}
