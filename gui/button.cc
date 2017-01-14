#include "gui/button.h"
#include "uistate.h"

#include "render/scalablesprite.h"
#include "render/spriterender.h"
#include "render/fonts.h"

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
  if( sprite_.get() != nullptr ) {
    renderer->DrawNinepatch(*sprite_.get(), GetRect().GetPosition());
  }
  if( text_.HasText() ) {
    const auto p = GetRect().GetAbsoluteCenterPos() - text_.GetText().GetExtents().GetRelativeCenterPos();
    text_.GetText().Draw(p);
  }
}

TextData& Button::Text() {
  return text_;
}

void Button::SetSprite(std::shared_ptr<ScalableSprite> sprite) {
  sprite_ = sprite;
}
