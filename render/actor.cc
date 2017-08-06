#include "render/actor.h"

#include "core/assert.h"

Actor::Actor(std::shared_ptr<CompiledMesh> mesh)
: mesh_(mesh)
, position_(vec3f::Origo())
, rotation_(quatf::Identity())
{
  Assert(mesh);
}

const vec3f& Actor::GetPosition()
{
  return position_;
}

const quatf& Actor::GetRotation()
{
  return rotation_;
}

void Actor::SetPosition(const vec3f& position)
{
  position_ = position;
}

void Actor::SetRotation(const quatf& rotation)
{
  rotation_ = rotation;
}

mat4f Actor::GetModelMatrix() const
{
  return mat4f::FromTranslation(position_) * rotation_.ToMat4();
}

void Actor::Render(const mat4f& projection_matrix, const mat4f& view_matrix)
{
  mesh_->Render(GetModelMatrix(), projection_matrix, view_matrix);
}
