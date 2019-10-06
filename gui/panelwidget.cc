#include "gui/panelwidget.h"

namespace euphoria::gui
{
    PanelWidget::PanelWidget(UiState* state) : Widget(state) {}

    void
    PanelWidget::OnSize()
    {
        container.DoLayout(GetClientRect());
    }

    core::Sizef
    PanelWidget::CalculateMinimumSize() const
    {
        return container.CalculateMinimumArea();
    }

    void
    PanelWidget::Step(float dt)
    {
        container.Step(dt);
    }

    void
    PanelWidget::Render(render::SpriteRenderer* renderer) const
    {
        container.Render(renderer);
    }
}  // namespace euphoria::gui
