#ifndef EUPHORIA_ACTOR_H
#define EUPHORIA_ACTOR_H

#include <memory>

#include "core/mat4.h"
#include "core/vec3.h"
#include "core/quat.h"
#include "core/rgb.h"
#include "render/compiledmesh.h"

class Actor
{
 public:
  Actor(const std::shared_ptr<CompiledMesh>& mesh);

  // todo: make poisition and rotation public and skip getters and setters

  const vec3f&
  GetPosition();

  const quatf&
  GetRotation();

  void
  SetPosition(const vec3f& position);

  void
  SetRotation(const quatf& rotation);

  void
  BeginMaterialOverride(unsigned int index);

  bool
  IsMaterialOverridden(unsigned int index) const;

  CompiledMeshMaterial*
  GetOverriddenMaterial(unsigned int index);

  void
  EndMaterialOverride(unsigned int index);

  mat4f
  GetModelMatrix() const;

  void
  Render(
      const mat4f& projection_matrix,
      const mat4f& view_matrix,
      const vec3f& camera,
      const Light& light);

  void
  BasicRender(
      const mat4f&                    projection_matrix,
      const mat4f&                    view_matrix,
      std::shared_ptr<MaterialShader> shader);


  std::shared_ptr<CompiledMesh> mesh_;
  vec3f                         position_;
  quatf                         rotation_;

 private:
  std::vector<std::shared_ptr<CompiledMeshMaterial>> overridden_materials_;

 public:
  bool has_outline;
  Rgb  outline_color;
};


#endif  // EUPHORIA_ACTOR_H
