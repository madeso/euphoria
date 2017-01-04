#include "gui/button.h"
#include "uistate.h"

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
