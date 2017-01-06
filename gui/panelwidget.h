#ifndef GUI_PANELWIDGET_H
#define GUI_PANELWIDGET_H

#include "gui/widget.h"
#include "gui/layoutcontainer.h"

class PanelWidget : Widget {
public:
  Sizei CalculateMinimumSize() const override;
  void Step() override;
  LayoutContainer container;
};

#endif  // GUI_PANELWIDGET_H