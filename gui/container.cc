#include "gui/container.h"
#include "gui/widget.h"

Container::Container() {
}

Container::~Container() {
}

void Container::Add(std::shared_ptr<Widget> widget) {
  widgets_.push_back(widget);
}

void Container::Step() {
  for(auto w: widgets_) {
    w->Step();
  }
}

void Container::Render(SpriteRenderer* renderer) const {
  for(auto w: widgets_) {
    w->Render(renderer);
  }
}
