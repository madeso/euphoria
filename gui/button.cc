#include "gui/button.h"
#include "uistate.h"

#include "render/scalablesprite.h"
#include "render/spriterender.h"
#include "render/fonts.h"
#include "skin.h"

Button::Button(UiState* state) : Widget(state) {}

Button::~Button() {}

void Button::Step() {
  if( GetRect().ContainsExclusive(GetState().mouse) ) {
    GetStatePtr()->SetHot(this);
    if( GetState().IsMouseDown() ) {
      GetStatePtr()->SetActive(this);
    }
  }

  const bool is_clicked = GetState().IsMouseDown()==false && IsHot() && IsActive();
  if( is_clicked ) {
    OnClicked();
  }
}

Sizef Button::CalculateMinimumSize() const {
  Sizef size = Sizef::FromWidthHeight(0,0);
  if( sprite_.get() != nullptr ) {
    const Sizef ms = sprite_->GetMinimumSize();
    size.SetWidth( size.GetWidth() + ms.GetWidth() );
    size.SetHeight( size.GetHeight() + ms.GetHeight() );
  }

  if( text_.HasText() ) {
    const auto e = text_.GetText().GetExtents();
    Sizef ms = Sizef::FromWidthHeight(e.GetWidth(), e.GetHeight());
    size.SetWidth( size.GetWidth() + ms.GetWidth() );
    size.SetHeight( size.GetHeight() + ms.GetHeight() );
  }

  return size;
}

void Button::Render(SpriteRenderer* renderer) const {
  if( skin_ ) {
    const ButtonState& state = (IsHot()?
                                (
                                    (IsActive()? skin_->button_active_hot : skin_->button_hot)
                                )
                                       : skin_->button_idle);

    const vec2f displace(state.dx, state.dy);

    if( sprite_.get() != nullptr ) {
      renderer->DrawNinepatch(*sprite_.get(), GetRect().GetPosition() + displace, 0.0f, vec2f(state.scale, state.scale), state.image_color);
    }
    if( text_.HasText() ) {
      const auto p = GetRect().GetAbsoluteCenterPos() - text_.GetText().GetExtents().GetRelativeCenterPos().GetFlippedY() + displace;
      text_.GetText().Draw(p, state.text_color);
    }
  }
}

TextData& Button::Text() {
  return text_;
}

void Button::SetSprite(std::shared_ptr<ScalableSprite> sprite) {
  sprite_ = sprite;
}

void Button::OnSize() {
  if( sprite_.get() != nullptr) {
    sprite_->SetSize(GetRect().GetSize());
  }
}

void Button::SetSkin(Skin* skin) {
  skin_ = skin;
}
