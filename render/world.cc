#include "render/world.h"

#include "render/materialshader.h"

void World::AddActor(std::shared_ptr<Actor> actor)
{
  actors_.push_back(actor);
}

void World::Render(MaterialShader* shader)
{
  for(const auto& actor: actors_)
  {
    // todo: instead of direct rendering, move to a material sorting/render command system
    shader->SetModel(actor->GetModelMatrix());
    actor->Render();
  }
}
