#ifndef EUPHORIA_WORLD_H
#define EUPHORIA_WORLD_H

#include <memory>
#include <vector>

#include "render/actor.h"
#include "render/light.h"

class Camera;
class Viewport;
class MaterialShader;


class World
{
 public:
  World();

  void
  AddActor(const std::shared_ptr<Actor>& actor);

  // todo: improve light support
  Light light;

  // todo: move to a better place
  std::shared_ptr<MaterialShader> outline_shader;

  void
  Render(const Viewport& viewport, const Camera& camera);

  bool use_outline = false;

 private:
  std::vector<std::shared_ptr<Actor>> actors_;
};


#endif  // EUPHORIA_WORLD_H
