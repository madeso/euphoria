#include "gui/panelwidget.h"

#include "gui/visitor.h"


namespace euphoria::gui
{
    PanelWidget::PanelWidget(UiState* state)
        : Widget(state)
    {
    }


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
    PanelWidget::Render(render::sprite_renderer* renderer) const
    {
        container.Render(renderer);
    }

    void
    PanelWidget::Visit(Visitor* visitor)
    {
        visitor->Visit(this);
    }
}

