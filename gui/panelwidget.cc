#include "gui/panelwidget.h"

#include <cassert>

PanelWidget::PanelWidget(UiState* state) : Widget(state) {
}

void PanelWidget::OnSize() {
  container.DoLayout(GetRect());
}

Sizef PanelWidget::CalculateMinimumSize() const {
  return container.CalculateMinimumArea();
}

void PanelWidget::Step() {
  assert(this);
  container.Step();
}

void PanelWidget::Render(SpriteRenderer* renderer) const {
  container.Render(renderer);
}
