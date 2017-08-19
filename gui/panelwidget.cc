#include "gui/panelwidget.h"

#include "core/assert.h"

PanelWidget::PanelWidget(UiState* state) : Widget(state) {
}

void PanelWidget::OnSize() {
  container.DoLayout(GetRect());
}

Sizef PanelWidget::CalculateMinimumSize() const {
  return container.CalculateMinimumArea();
}

void PanelWidget::Step(float dt) {
  ASSERT(this);
  container.Step(dt);
}

void PanelWidget::Render(SpriteRenderer* renderer) const {
  container.Render(renderer);
}
