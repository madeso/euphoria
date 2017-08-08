#ifndef EUPHORIA_WORLD_H
#define EUPHORIA_WORLD_H

#include <memory>
#include <vector>

#include "render/actor.h"
#include "render/light.h"

class Camera;
class Viewport;
class MaterialShader;


class World {
 public:
  void AddActor(std::shared_ptr<Actor> actor);

  // todo: improve light support
  Light light;

  void Render(const Viewport& viewport, const Camera& camera);

 private:
  std::vector<std::shared_ptr<Actor>> actors_;
};


#endif //EUPHORIA_WORLD_H
