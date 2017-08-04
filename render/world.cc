#include "render/world.h"

#include "render/materialshader.h"

#include "render/camera.h"
#include "render/viewport.h"

#include "core/assert.h"

void World::AddActor(std::shared_ptr<Actor> actor)
{
  actors_.push_back(actor);
}

void World::Render(const Viewport& viewport, const Camera& camera, MaterialShader* shader)
{
  Assert(viewport.IsActive());
  shader->SetProjection(camera.CalculateProjectionMatrix(viewport.GetAspectRatio()));
  shader->SetView(camera.CalculateViewMatrix());
  for(const auto& actor: actors_)
  {
    // todo: instead of direct rendering, move to a material sorting/render command system
    shader->SetModel(actor->GetModelMatrix());
    actor->Render();
  }
}
