#include "render/world.h"

#include "render/materialshader.h"

#include "render/camera.h"
#include "render/viewport.h"

#include "core/assert.h"

void
World::AddActor(const std::shared_ptr<Actor>& actor)
{
  actors_.push_back(actor);
}

void
World::Render(const Viewport& viewport, const Camera& camera)
{
  Assert(viewport.IsActive());

  const auto projection_matrix =
      camera.CalculateProjectionMatrix(viewport.GetAspectRatio());
  const auto view_matrix = camera.CalculateViewMatrix();

  for(const auto& actor : actors_)
  {
    // todo: instead of direct rendering, move to a material sorting/render
    // command system
    // general design: http://realtimecollisiondetection.net/blog/?p=86
    // distance from camera:
    // https://gamedev.stackexchange.com/questions/56810/how-do-games-deal-with-z-sorting-partially-transparent-foliage-textures
    // distance in integer:
    // http://aras-p.info/blog/2014/01/16/rough-sorting-by-depth/
    // useful?
    // https://gamedev.stackexchange.com/questions/45626/how-to-organize-rendering
    actor->Render(projection_matrix, view_matrix, camera.GetPosition(), light);
  }
}
