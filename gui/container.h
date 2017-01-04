#ifndef GUI_CONTAINER_H
#define GUI_CONTAINER_H

#include <memory>
#include <vector>

class Widget;

class Container {
public:
  Container();
  ~Container();

  void Add(Widget* widget);

  void Step();

private:
  std::vector<std::shared_ptr<Widget>> widgets_;
};

#endif  // GUI_CONTAINER_H
