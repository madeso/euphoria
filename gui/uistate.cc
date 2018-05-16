#include "gui/uistate.h"

UiState::UiState()
    : mouse(0, 0)
    , mouse_down(false)
    , hot(nullptr)
    , active(nullptr)
    , has_active(false)
{
}

void
UiState::Begin()
{
  hot = nullptr;
}

void
UiState::End()
{
  if(IsMouseDown() == false)
  {
    active     = nullptr;
    has_active = false;
  }
  else
  {
    if(has_active == false && active == nullptr)
    {
      has_active = true;
    }
  }
}

void
UiState::SetHot(Widget* w)
{
  hot = w;
}

void
UiState::SetActive(Widget* w)
{
  if(has_active == false && active == nullptr)
  {
    active     = w;
    has_active = true;
  }
}

bool
UiState::IsMouseDown() const
{
  return mouse_down;
}