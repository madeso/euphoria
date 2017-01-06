#include "gui/widget.h"

#include "gui/uistate.h"

#include <cassert>

Widget::Widget(UiState* state) : state_(state) {
}

Widget::~Widget() {
}

bool Widget::IsActive() const {
  assert(this);
  return state_->active == this;
}

bool Widget::IsHot() const {
  assert(this);
  return state_->hot == this;
}

Recti Widget::GetRect() const {
  assert(this);
  return rect_;
}

void Widget::SetRect(const Recti& r) {
  assert(this);
  rect_ = r;
}

LayoutData& Widget::Layout() {
  assert(this);
  return layout_;
}

const LayoutData& Widget::Layout() const {
  assert(this);
  return layout_;
}

const UiState& Widget::GetState() const {
  assert(this);
  return *state_;
}

UiState* Widget::GetStatePtr() {
  assert(this);
  return state_;
}
