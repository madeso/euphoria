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
      renderer->DrawNinepatch(
          *sprite_.get(),
          GetBackgroundRect().TopLeft() + position_displacement_.GetValue(),
          vec2f(scale_.GetValue(), scale_.GetValue()),
          image_color_.GetValue());
    }
    if(text_.HasText())
    {
      const auto p = GetClientRect().GetAbsoluteCenterPos() -
                     text_.GetText()
                         .GetExtents(scale_.GetValue())
                         .GetRelativeCenterPosFromBottomLeft()
                         .GetFlippedY() +
                     position_displacement_.GetValue();
      text_.GetText().Draw(p, text_color_.GetValue(), scale_.GetValue());
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
  if(sprite_.get() != nullptr)
  {
    sprite_->SetSize(GetBackgroundRect().GetSize());
  }
}

void
Button::SetSkin(Skin* skin)
{
  skin_ = skin;
}
