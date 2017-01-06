#ifndef GUI_WIDGET_H
#define GUI_WIDGET_H

#include "core/rect.h"
#include "gui/layoutdata.h"

class UiState;

class Widget {
public:
  explicit Widget(UiState* state);
  virtual ~Widget();

  bool IsActive() const;
  bool IsHot() const;

  virtual void Step() = 0;

  Recti GetRect() const;
  void SetRect(const Recti& r);

  LayoutData& Layout();
  const LayoutData& Layout() const;

protected:
  const UiState& GetState() const;
  UiState* GetStatePtr();

private:
  UiState* state_;
  LayoutData layout_;
  Recti rect_;
};

#endif  // GUI_WIDGET_H