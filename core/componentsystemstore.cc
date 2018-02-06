#include "core/componentsystemstore.h"

#include "core/componentsystem.h"

#include "core/assert.h"

void
ComponentSystemStore::Add(
    const std::string& name, std::shared_ptr<ComponentSystem> system)
{
  const int size = systems.size();
  systems.emplace_back(system);
  names.emplace(name, size);
}

ComponentSystemType
ComponentSystemStore::Lookup(const std::string& name) const
{
  const auto found = names.find(name);
  if(found == names.end())
  {
    return ComponentSystemType{nullptr, 0};
  }
  else
  {
    return ComponentSystemType{this, found->second};
  }
}

void
ComponentSystemStore::Create(ComponentSystemType type, IdGenerator::ID object)
{
  ASSERT(this == type.owner);
  systems[type.index]->Create(object);
}
