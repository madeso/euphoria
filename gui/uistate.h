#ifndef GUI_UISTATE_H
#define GUI_UISTATE_H

#include "core/vec2.h"

class Widget;

class UiState
{
 public:
  UiState();

  vec2f mouse;
  bool  mouse_down;

  Widget* hot;
  Widget* active;
  bool    has_active;

  void
  Begin();
  void
  End();

  void
  SetHot(Widget* w);
  void
  SetActive(Widget* w);
  bool
  IsMouseDown() const;
};

#endif  // GUI_UISTATE_H
