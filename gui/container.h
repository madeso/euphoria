#ifndef GUI_CONTAINER_H
#define GUI_CONTAINER_H

#include <memory>
#include <vector>

class Widget;
class SpriteRenderer;

class Container
{
 public:
  Container();
  ~Container();

  bool
  HasWidgets() const;

  void
  Add(std::shared_ptr<Widget> widget);

  void
  Step(float dt);

  void
  Render(SpriteRenderer* renderer) const;

 protected:
  std::vector<std::shared_ptr<Widget>> widgets_;
};

#endif  // GUI_CONTAINER_H
