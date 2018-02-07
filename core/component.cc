#include "core/component.h"

#include "core/assert.h"

void
ComponentStore::Add(const std::string& name, std::shared_ptr<Components> system)
{
  const int size = components.size();
  components.emplace_back(system);
  names.emplace(name, size);
}

ComponentSystemType
ComponentStore::Lookup(const std::string& name) const
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
ComponentStore::Create(ComponentSystemType type, IdGenerator::ID object)
{
  Get(type).Create(object);
}

std::vector<IdGenerator::ID>
ComponentStore::ListObjectsWithComponents(
    const std::vector<ComponentSystemType>& types)
{
  ASSERT(!types.empty());

  // get all objects with the first component
  std::vector<IdGenerator::ID> potential = Get(types[0]).List();

  const std::vector<ComponentSystemType> other_types(
      types.begin() + 1, types.end());
  // if there was only one component requirement, return all the objects with
  // the first component
  if(other_types.empty())
  {
    return potential;
  }

  // get the components for easier access later on
  std::vector<std::shared_ptr<Components>> other_components(other_types.size());
  for(auto t : other_types)
  {
    other_components.emplace_back(GetPtr(t));
  }

  std::vector<IdGenerator::ID> ret(potential.size());

  // for each potential, see if the potential has the other components
  for(const auto id : potential)
  {
    bool valid = true;
    for(const auto c : other_components)
    {
      if(!c->Has(id))
      {
        valid = false;
        break;
      }
    }
    if(valid)
    {
      ret.emplace_back(id);
    }
  }

  return ret;
}

Components&
ComponentStore::Get(ComponentSystemType type)
{
  return *GetPtr(type);
}

std::shared_ptr<Components>
ComponentStore::GetPtr(ComponentSystemType type)
{
  ASSERT(this == type.owner);
  return components[type.index];
}
