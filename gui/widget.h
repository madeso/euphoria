#ifndef GUI_WIDGET_H
#define GUI_WIDGET_H

#include "core/rect.h"
#include "core/size.h"

#include "gui/layoutdata.h"

class UiState;
class SpriteRenderer;

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

  Sizei GetPreferredSize() const;

  virtual Sizei CalculateMinimumSize() const = 0;

  virtual void Render(SpriteRenderer* renderer) const = 0;

protected:
  const UiState& GetState() const;
  UiState* GetStatePtr();

private:
  UiState* state_;
  LayoutData layout_;
  Recti rect_;
};

#endif  // GUI_WIDGET_H