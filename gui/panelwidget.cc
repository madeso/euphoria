#include "gui/panelwidget.h"

PanelWidget::PanelWidget(UiState* state)
    : Widget(state)
{
}

void
PanelWidget::OnSize()
{
  container.DoLayout(GetRect());
}

Sizef
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
PanelWidget::Render(SpriteRenderer* renderer) const
{
  container.Render(renderer);
}
