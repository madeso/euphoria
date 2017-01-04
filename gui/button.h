#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H

#include <core/rect.h>
#include "gui/widget.h"

class Button : public Widget {
public:
  Button(UiState* state);
  ~Button();

  virtual void OnClicked() = 0;

  void Step() override;
};

#endif  // GUI_BUTTON_H
