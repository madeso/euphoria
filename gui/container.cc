#include "gui/container.h"
#include "gui/widget.h"

Container::Container() {
}

Container::~Container() {
}

void Container::Add(Widget* widget) {
  std::shared_ptr<Widget> widget_ptr(widget);
  widgets_.push_back(widget_ptr);
}

void Container::Step() {
  for(auto w: widgets_) {
    w->Step();
  }
}
