#include "gui/widget.h"

#include "gui/uistate.h"

#include "core/assert.h"

Widget::Widget(UiState* state) : state_(state) {
}

Widget::~Widget() {
}

bool Widget::IsActive() const {
  Assert(this);
  return state_->active == this;
}

bool Widget::IsHot() const {
  Assert(this);
  return state_->hot == this;
}

void Widget::OnSize() {
  Assert(this);
}

Rectf Widget::GetRect() const {
  Assert(this);
  return rect_;
}

void Widget::SetRect(const Rectf& r) {
  Assert(this);
  rect_ = r;
  OnSize();
}

LayoutData& Widget::Layout() {
  Assert(this);
  return layout_;
}

const LayoutData& Widget::Layout() const {
  Assert(this);
  return layout_;
}

Sizef Widget::GetPreferredSize() const {
  const Sizef min = this->CalculateMinimumSize();
  return Sizef::FromWidthHeight( Max(min.GetWidth(), Layout().GetPreferredWidth()), Max(min.GetHeight(), Layout().GetPreferredHeight()) );
}

const UiState& Widget::GetState() const {
  Assert(this);
  return *state_;
}

UiState* Widget::GetStatePtr() {
  Assert(this);
  return state_;
}
