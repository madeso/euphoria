#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H

#include <core/rect.h>
#include "gui/widget.h"
#include "gui/textdata.h"

#include <memory>

class ScalableSprite;
class Skin;

class Button : public Widget {
public:
  Button(UiState* state);
  ~Button();

  virtual void OnClicked() = 0;

  void Step() override;
  Sizef CalculateMinimumSize() const override;

  void Render(SpriteRenderer* renderer) const override;

  TextData& Text();

  void SetSprite(std::shared_ptr<ScalableSprite> sprite);

  void OnSize() override;
  void SetSkin(Skin* skin);

private:
  std::shared_ptr<ScalableSprite> sprite_;
  TextData text_;
  Skin* skin_;
};

#endif  // GUI_BUTTON_H
