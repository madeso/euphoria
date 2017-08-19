#include "gui/widget.h"

#include "gui/uistate.h"

#include "core/assert.h"

Widget::Widget(UiState* state) : state_(state) {
}

Widget::~Widget() {
}

bool Widget::IsActive() const {
  ASSERT(this);
  return state_->active == this;
}

bool Widget::IsHot() const {
  ASSERT(this);
  return state_->hot == this;
}

void Widget::OnSize() {
  ASSERT(this);
}

Rectf Widget::GetRect() const {
  ASSERT(this);
  return rect_;
}

void Widget::SetRect(const Rectf& r) {
  ASSERT(this);
  rect_ = r;
  OnSize();
}

LayoutData& Widget::Layout() {
  ASSERT(this);
  return layout_;
}

const LayoutData& Widget::Layout() const {
  ASSERT(this);
  return layout_;
}

Sizef Widget::GetPreferredSize() const {
  const Sizef min = this->CalculateMinimumSize();
  return Sizef::FromWidthHeight( Max(min.GetWidth(), Layout().GetPreferredWidth()), Max(min.GetHeight(), Layout().GetPreferredHeight()) );
}

const UiState& Widget::GetState() const {
  ASSERT(this);
  return *state_;
}

UiState* Widget::GetStatePtr() {
  ASSERT(this);
  return state_;
}
