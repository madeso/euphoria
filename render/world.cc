#include "render/world.h"

#include "render/materialshader.h"

#include "render/camera.h"
#include "render/viewport.h"

#include "core/assert.h"

void World::AddActor(std::shared_ptr<Actor> actor)
{
  actors_.push_back(actor);
}

void World::Render(const Viewport& viewport, const Camera& camera)
{
  Assert(viewport.IsActive());

  const auto projection_matrix = camera.CalculateProjectionMatrix(viewport.GetAspectRatio());
  const auto view_matrix = camera.CalculateViewMatrix();

  for(const auto& actor: actors_)
  {
    // todo: instead of direct rendering, move to a material sorting/render command system
    actor->Render(projection_matrix, view_matrix);
  }
}
