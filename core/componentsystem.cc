#include "core/componentsystem.h"

ComponentSystem::ComponentSystem(const std::string& the_name)
    : name(the_name)
{
}

void
ComponentSystemStore::Add(std::shared_ptr<ComponentSystem> system)
{
  systems.emplace_back(system);
}

void
ComponentSystemUpdateStore::Update(EntReg* reg, float dt) const
{
  for(const auto s : systems)
  {
    s->Update(reg, dt);
  }
}

void
ComponentSystemSpriteDrawStore::Draw(
    EntReg* reg, SpriteRenderer* renderer) const
{
  for(const auto s : systems)
  {
    s->Draw(reg, renderer);
  }
}

void
Systems::AddAndRegister(std::shared_ptr<ComponentSystem> system)
{
  store.Add(system);
  system->RegisterCallbacks(this);
}

World::World(Systems* sys)
: systems(sys)
{}

void
World::Update(float dt)
{
  systems->update.Update(&reg, dt);
}

void
World::Draw(SpriteRenderer* renderer)
{
  systems->spriteDraw.Draw(&reg, renderer);
}
