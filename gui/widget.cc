#include "gui/widget.h"

#include "gui/uistate.h"

Widget::Widget(UiState* state) : state_(state) {
}

Widget::~Widget() {
}

bool Widget::IsActive() const {
  return state_->active == this;
}

bool Widget::IsHot() const {
  return state_->hot == this;
}

Rect<int> Widget::GetRect() const {
  return rect_;
}

const UiState& Widget::GetState() const {
  return *state_;
}

UiState* Widget::GetStatePtr() {
  return state_;
}
