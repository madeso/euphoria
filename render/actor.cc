#include "render/actor.h"

#include "core/assert.h"

Actor::Actor(const std::shared_ptr<CompiledMesh>& mesh)
    : mesh_(mesh)
    , position_(vec3f::Zero())
    , rotation_(quatf::Identity())
    , overridden_materials_(mesh->GetNoOverriddenMaterials())
    , has_outline(false)
    , outline_color(Color::White)
{
  ASSERT(mesh);
}

const vec3f&
Actor::GetPosition()
{
  return position_;
}

const quatf&
Actor::GetRotation()
{
  return rotation_;
}

void
Actor::SetPosition(const vec3f& position)
{
  position_ = position;
}

void
Actor::SetRotation(const quatf& rotation)
{
  rotation_ = rotation;
}

void
Actor::BeginMaterialOverride(unsigned int index)
{
  ASSERT(index < overridden_materials_.size());
  ASSERT(!IsMaterialOverridden(index));
  overridden_materials_[index] =
      std::make_shared<CompiledMeshMaterial>(mesh_->materials[index]);
}

bool
Actor::IsMaterialOverridden(unsigned int index) const
{
  ASSERT(index < overridden_materials_.size());
  return overridden_materials_[index] != nullptr;
}

CompiledMeshMaterial*
Actor::GetOverriddenMaterial(unsigned int index)
{
  ASSERT(index < overridden_materials_.size());
  ASSERT(IsMaterialOverridden(index));
  return overridden_materials_[index].get();
}

void
Actor::EndMaterialOverride(unsigned int index)
{
  ASSERT(index < overridden_materials_.size());
  ASSERT(!IsMaterialOverridden(index));
  overridden_materials_[index] = nullptr;
}

mat4f
Actor::GetModelMatrix() const
{
  return mat4f::FromTranslation(vec3f::FromOrigoTo(position_)) *
         rotation_.ToMat4();
}

void
Actor::Render(
    const mat4f& projection_matrix,
    const mat4f& view_matrix,
    const vec3f& camera,
    const Light& light)
{
  mesh_->Render(
      GetModelMatrix(),
      projection_matrix,
      view_matrix,
      camera,
      light,
      overridden_materials_);
}

void
Actor::BasicRender(
    const mat4f&                    projection_matrix,
    const mat4f&                    view_matrix,
    std::shared_ptr<MaterialShader> shader)
{
  mesh_->BasicRender(GetModelMatrix(), projection_matrix, view_matrix, shader);
}
