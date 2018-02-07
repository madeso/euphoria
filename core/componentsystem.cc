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
ComponentSystemUpdateStore::Update(float dt) const
{
  for(const auto s : systems)
  {
    s->Update(dt);
  }
}
