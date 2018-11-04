#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H

#include "core/rect.h"
#include "core/rgb.h"
#include "core/vec2.h"
#include "core/interpolate.h"
#include "gui/widget.h"
#include "gui/textdata.h"

#include <memory>


class ScalableSprite;
class Skin;
class ButtonState;

class Button : public Widget
{
 public:
  Button(UiState* state);
  ~Button();

  virtual void
  OnClicked() = 0;

  void
  Step(float dt) override;
  Sizef
  CalculateMinimumSize() const override;

  void
  Render(SpriteRenderer* renderer) const override;

  TextData&
  Text();

  void
  SetSprite(std::shared_ptr<ScalableSprite> sprite);

  void
  OnSize() override;
  void
  SetSkin(Skin* skin);

 private:
  ButtonState*                    last_state_;
  std::shared_ptr<ScalableSprite> sprite_;
  TextData                        text_;
  Skin*                           skin_;

  Interpolate<float, FloatTransform>     scale_;
  Interpolate<Rgb, RgbTransform>         image_color_;
  Interpolate<Rgb, RgbTransform>         text_color_;
  Interpolate<point2f, Point2fTransform> position_displacement_;
};

#endif  // GUI_BUTTON_H
