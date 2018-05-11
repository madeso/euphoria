#include "engine/dukregistry.h"

#include <algorithm>

struct ScriptComponent : public Component
{
  COMPONENT_CONSTRUCTOR_DEFINITION(ScriptComponent)

  DukValue val;
};

COMPONENT_CONSTRUCTOR_IMPLEMENTATION(ScriptComponent)

DukRegistry::DukRegistry(EntReg* r, Components* c)
    : reg(r)
    , components(c)
{
}

ComponentId
DukRegistry::getPosition2dId()
{
  return components->position2;
}

ComponentId
DukRegistry::getSpriteId()
{
  return components->sprite;
}

ComponentId
DukRegistry::CreateNewId(const std::string& name, const FunctionVar& fv)
{
  const auto id        = reg->NewComponentType(name);
  scriptComponents[id] = fv;
  return id;
}

bool
DukRegistry::GetCustomComponentByName(const std::string& name, ComponentId* id)
{
  return reg->GetCustomComponentByName(name, id);
}

std::vector<EntityId>
DukRegistry::entities(const std::vector<ComponentId>& types)
{
  return reg->View(types);
}

DukValue
DukRegistry::GetProperty(EntityId ent, ComponentId comp)
{
  ASSERT(scriptComponents.find(comp) != scriptComponents.end());
  auto c = reg->GetComponent(ent, comp);
  if(c.get() == nullptr)
  {
    return DukValue{};
  }
  else
  {
    return static_cast<ScriptComponent*>(c.get())->val;
  }
}

void
DukRegistry::SetProperty(EntityId ent, ComponentId comp, DukValue value)
{
  // todo: move to a better construct
  ASSERT(scriptComponents.find(comp) != scriptComponents.end());

  auto c = reg->GetComponent(ent, comp);
  if(c.get() == nullptr)
  {
    auto d = std::make_shared<ScriptComponent>();
    d->val = value;
    reg->AddComponent(ent, comp, d);
  }
  else
  {
    static_cast<ScriptComponent*>(c.get())->val = value;
  }
}

DukValue
DukRegistry::CreateComponent(ComponentId comp, Context* ctx)
{
  auto res = scriptComponents.find(comp);
  ASSERT(res != scriptComponents.end());
  if(res == scriptComponents.end())
  {
    return DukValue{};
  }

  if(!res->second.IsValid())
  {
    return DukValue{};
  }

  auto val = res->second.Call<DukValue>(ctx);
  ASSERT(val.IsValid());
  // todo: need to increase refcount on val here like functions, right?
  return val;
}
