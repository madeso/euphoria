#include "gui/button.h"
#include "uistate.h"

#include "render/scalablesprite.h"
#include "render/spriterender.h"
#include "render/fonts.h"
#include "skin.h"

Button::Button(UiState* state)
    : Widget(state)
    , last_state_(nullptr)
    , scale_(1.0f)
    , image_color_(Rgb(1.0f))
    , text_color_(Rgb(1.0f))
    , position_displacement_(vec2f(0, 0))
{
}

Button::~Button()
{
}

void
Button::Step(float dt)
{
  if(GetBackgroundRect().ContainsExclusive(GetState().mouse))
  {
    GetStatePtr()->SetHot(this);
    if(GetState().IsMouseDown())
    {
      GetStatePtr()->SetActive(this);
    }
  }

  const bool is_clicked =
      GetState().IsMouseDown() == false && IsHot() && IsActive();
  if(is_clicked)
  {
    OnClicked();
  }

  if(skin_)
  {
    ButtonState* state =
        (IsHot()
             ? ((IsActive() ? &skin_->button_active_hot : &skin_->button_hot))
             : &skin_->button_idle);
    if(last_state_ != state)
    {
      last_state_ = state;
      scale_.Clear().Add(
          state->interpolationSize, state->scale, state->interpolationSizeTime);
      image_color_.Clear().Add(
          state->interpolationColor,
          state->image_color,
          state->interpolationColorTime);
      text_color_.Clear().Add(
          state->interpolationColor,
          state->text_color,
          state->interpolationColorTime);
      position_displacement_.Clear().Add(
          state->interpolationPosition,
          vec2f(state->dx, state->dy),
          state->interpolationPositionTime);
    }

    scale_.Update(dt);
    image_color_.Update(dt);
    text_color_.Update(dt);
    position_displacement_.Update(dt);
    text_.SetSize(skin_->text_size * scale_.GetValue());
  }
}

Sizef
Button::CalculateMinimumSize() const
{
  Sizef size = Sizef::FromWidthHeight(0, 0);
  if(sprite_.get() != nullptr)
  {
    const Sizef ms = sprite_->GetMinimumSize();
    size.SetWidth(size.GetWidth() + ms.GetWidth());
    size.SetHeight(size.GetHeight() + ms.GetHeight());
  }

  if(text_.HasText())
  {
    const auto e  = text_.GetText().GetExtents();
    Sizef      ms = Sizef::FromWidthHeight(e.GetWidth(), e.GetHeight());
    size.SetWidth(size.GetWidth() + ms.GetWidth());
    size.SetHeight(size.GetHeight() + ms.GetHeight());
  }

  return size;
}

void
Button::Render(SpriteRenderer* renderer) const
{
  if(skin_)
  {
    /*
    const ButtonState& state = (IsHot()?
                                (
                                    (IsActive()? skin_->button_active_hot :
    skin_->button_hot)
                                )
                                       : skin_->button_idle);
    */
    if(sprite_.get() != nullptr)
    {
      const auto rect = GetBackgroundRect();

      ASSERTX(scale_.GetValue() > 0, scale_.GetValue());
      const auto scaled = rect.GetScaledAroundCenterCopy(scale_.GetValue());
      ASSERTX(scaled.GetWidth() > 0, scaled.GetWidth());
      ASSERTX(scaled.GetHeight() > 0, scaled.GetHeight());
      renderer->DrawNinepatch(
          *sprite_.get(),
          scaled.OffsetCopy(position_displacement_.GetValue()),
          image_color_.GetValue());
    }
    if(text_.HasText())
    {
      const auto ex = text_.GetText().GetExtents();
      // todo: render text at client rect center
      const auto p = ex.CenterInsideOther(GetClientRect()).GetBottomLeft() +
                     position_displacement_.GetValue();
      text_.GetText().Draw(renderer, p, text_color_.GetValue());
    }
  }
}

TextData&
Button::Text()
{
  return text_;
}

void
Button::SetSprite(std::shared_ptr<ScalableSprite> sprite)
{
  sprite_ = sprite;
}

void
Button::OnSize()
{
}

void
Button::SetSkin(Skin* skin)
{
  skin_ = skin;
  if(skin_)
  {
    text_.SetSize(skin_->text_size);
  }
}
