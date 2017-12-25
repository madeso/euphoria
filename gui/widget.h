#ifndef GUI_WIDGET_H
#define GUI_WIDGET_H

#include <string>

#include "core/rect.h"
#include "core/size.h"

#include "gui/layoutdata.h"

class UiState;
class SpriteRenderer;

class Widget {
public:
  std::string name;

  explicit Widget(UiState* state);
  virtual ~Widget();

  bool IsActive() const;
  bool IsHot() const;

  virtual void Step(float dt) = 0;
  virtual void OnSize();

  Rectf GetRect() const;
  void SetRect(const Rectf& r);

  Sizef GetPreferredSize() const;

  virtual Sizef CalculateMinimumSize() const = 0;

  virtual void Render(SpriteRenderer* renderer) const = 0;

protected:
  const UiState& GetState() const;
  UiState* GetStatePtr();

private:
  UiState* state_;

public:
  LayoutData layout;

private:
  Rectf rect_;
};

#endif  // GUI_WIDGET_H
