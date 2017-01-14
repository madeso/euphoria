#ifndef GUI_PANELWIDGET_H
#define GUI_PANELWIDGET_H

#include "gui/widget.h"
#include "gui/layoutcontainer.h"

class PanelWidget : public Widget {
public:
  PanelWidget(UiState* state);

  void Render(SpriteRenderer* renderer) const override;

  void OnSize() override;
  Sizef CalculateMinimumSize() const override;
  void Step() override;
  LayoutContainer container;
};

#endif  // GUI_PANELWIDGET_H