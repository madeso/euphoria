#ifndef GUI_PANELWIDGET_H
#define GUI_PANELWIDGET_H

#include "gui/widget.h"
#include "gui/layoutcontainer.h"

namespace euphoria::gui
{
    struct PanelWidget : public Widget
    {
        PanelWidget(UiState* state);

        void
        Render(render::SpriteRenderer* renderer) const override;

        void
        OnSize() override;

        core::Sizef
        CalculateMinimumSize() const override;

        void
        Step(float dt) override;

        LayoutContainer container;
    };
}

#endif  // GUI_PANELWIDGET_H

