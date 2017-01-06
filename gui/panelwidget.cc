#include "gui/panelwidget.h"

#include <cassert>

Sizei PanelWidget::CalculateMinimumSize() const {
  return container.CalculateMinimumArea();
}

void PanelWidget::Step() {
  assert(this);
  container.Step();
}
