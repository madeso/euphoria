#include "render/actor.h"

#include "core/assert.h"

Actor::Actor(std::shared_ptr<CompiledMesh> mesh)
: mesh_(mesh)
, position_(vec3f::Origo())
, rotation_(quatf::Identity())
{
  Assert(mesh);
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

void Actor::Render()
{
  mesh_->Render();
}
