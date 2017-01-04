#ifndef GUI_UI_H
#define GUI_UI_H

#include "gui/container.h"
#include "gui/uistate.h"

class Ui {
public:
  void Step();

  UiState state;
  Container container;
};

#endif  // GUI_UI_H
