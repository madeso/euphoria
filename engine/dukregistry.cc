#include "engine/dukregistry.h"

constexpr ComponentId ComponentIdStart = 100;

DukRegistry::DukRegistry(EntReg* r)
    : reg(r)
    , last_script_id(ComponentIdStart + 1)
{
  add<CPosition2>();
  add<CSprite>();
}

ComponentId
DukRegistry::getPosition2dId()
{
  return reg->component<CPosition2>();
}

ComponentId
DukRegistry::getSpriteId()
{
  return reg->component<CSprite>();
}

ComponentId
DukRegistry::CreateNewId(const std::string& name)
{
  ComponentId id = last_script_id;
  last_script_id += 1;

  name_to_component.insert(std::make_pair(name, id));
  return id;
}

bool
DukRegistry::GetCustomComponentByName(const std::string& name, ComponentId* id)
{
  ASSERT(id);
  const auto found_component = name_to_component.find(name);
  if(found_component == name_to_component.end())
  {
    return false;
  }
  else
  {
    *id = found_component->second;
    return true;
  }
}

std::vector<EntityId>
DukRegistry::entities(const std::vector<ComponentId>& types)
{
  std::vector<EntityId> ret;
  reg->each([this, types, &ret](auto ent) {
    if(this->hasEntityComponent(types, ent))
    {
      ret.push_back(ent);
    }
  });
  return ret;
}

DukValue
DukRegistry::GetProperty(EntityId ent, ComponentId comp)
{
  CScript& s = reg->accomodate<CScript>(ent);
  return s.properties[comp];
}

void
DukRegistry::SetProperty(EntityId ent, ComponentId comp, DukValue value)
{
  CScript& s         = reg->accomodate<CScript>(ent);
  s.properties[comp] = value;
}

bool
DukRegistry::hasEntityComponent(ComponentId t, EntityId ent)
{
  if(t > ComponentIdStart)
  {
    if(!reg->has<CScript>(ent))
    {
      return false;
    }
    CScript& comp = reg->get<CScript>(ent);
    auto     f    = comp.properties.find(t);
    return f != comp.properties.end();
  }
  auto f = comps.find(t);
  if(f == comps.end())
  {
    return false;
  }
  else
  {
    return f->second->has(ent, *reg);
  }
}

bool
DukRegistry::hasEntityComponent(
    const std::vector<ComponentId>& types, EntityId ent)
{
  for(auto t : types)
  {
    if(false == hasEntityComponent(t, ent))
    {
      return false;
    }
  }

  return true;
}
