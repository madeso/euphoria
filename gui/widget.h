#ifndef GUI_WIDGET_H
#define GUI_WIDGET_H

#include "core/rect.h"

class UiState;

class Widget {
public:
  explicit Widget(UiState* state);
  virtual ~Widget();

  bool IsActive() const;
  bool IsHot() const;

  virtual void Step() = 0;

  Rect<int> GetRect() const;

protected:
  const UiState& GetState() const;
  UiState* GetStatePtr();

private:
  UiState* state_;
  Rect<int> rect_;
};

#endif  // GUI_WIDGET_H