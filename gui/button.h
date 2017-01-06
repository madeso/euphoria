#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H

#include <core/rect.h>
#include "gui/widget.h"

#include <memory>



class ScalableSprite;

class Button : public Widget {
public:
  Button(UiState* state);
  ~Button();

  virtual void OnClicked() = 0;

  void Step() override;
  Sizef CalculateMinimumSize() const override;

  void Render(SpriteRenderer* renderer) const override;

private:
  std::shared_ptr<ScalableSprite> sprite_;
};

#endif  // GUI_BUTTON_H
