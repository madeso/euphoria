#include "gui/container.h"
#include "gui/widget.h"

namespace euphoria::gui
{
  Container::Container()
  {
  }

  Container::~Container()
  {
  }

  bool
  Container::HasWidgets() const
  {
    return !widgets_.empty();
  }

  void
  Container::Add(std::shared_ptr<Widget> widget)
  {
    widgets_.push_back(widget);
  }

  void
  Container::Step(float dt)
  {
    for(const auto& w : widgets_)
    {
      w->Step(dt);
    }
  }

  void
  Container::Render(render::SpriteRenderer* renderer) const
  {
    for(const auto& w : widgets_)
    {
      w->Render(renderer);
    }
  }
}
