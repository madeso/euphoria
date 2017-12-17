#include "gui/widget.h"

#include "core/log.h"

#include "gui/uistate.h"

LOG_SPECIFY_DEFAULT_LOGGER("gui.widget")

Widget::Widget(UiState* state)
    : state_(state)
{
}

Widget::~Widget()
{
}

bool
Widget::IsActive() const
{
  return state_->active == this;
}

bool
Widget::IsHot() const
{
  return state_->hot == this;
}

void
Widget::OnSize()
{
}

Rectf
Widget::GetRect() const
{
  return rect_;
}

void
Widget::SetRect(const Rectf& r)
{
  LOG_INFO("Setting gui rect" << r);
  rect_ = r;
  OnSize();
}

LayoutData&
Widget::Layout()
{
  return layout_;
}

const LayoutData&
Widget::Layout() const
{
  return layout_;
}

Sizef
Widget::GetPreferredSize() const
{
  const Sizef min = this->CalculateMinimumSize();
  return Sizef::FromWidthHeight(
      Max(min.GetWidth(), Layout().GetPreferredWidth()),
      Max(min.GetHeight(), Layout().GetPreferredHeight()));
}

const UiState&
Widget::GetState() const
{
  return *state_;
}

UiState*
Widget::GetStatePtr()
{
  return state_;
}
