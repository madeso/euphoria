#ifndef EUPHORIA_ACTOR_H
#define EUPHORIA_ACTOR_H

#include <memory>

#include "core/mat4.h"
#include "core/vec3.h"
#include "core/quat.h"
#include "render/compiledmesh.h"

class Actor {
 public:
  Actor(std::shared_ptr<CompiledMesh> mesh);

  const vec3f& GetPosition();
  const quatf& GetRotation();

  void SetPosition(const vec3f& position);
  void SetRotation(const quatf& rotation);

  mat4f GetModelMatrix() const;
  void Render();

 private:
  std::shared_ptr<CompiledMesh> mesh_;
  vec3f position_;
  quatf rotation_;
};


#endif //EUPHORIA_ACTOR_H
