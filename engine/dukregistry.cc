#include "engine/dukregistry.h"

#include <algorithm>

struct ScriptComponent : public Component
{
  COMPONENT_CONSTRUCTOR_DEFINITION(ScriptComponent)

  sol::table val;
};

TYPEID_SETUP_TYPE(ScriptComponent);
COMPONENT_CONSTRUCTOR_IMPLEMENTATION(ScriptComponent)

DukRegistry::DukRegistry(EntReg* r, Components* c)
    : reg(r)
    , components(c)
{
}

ComponentId
DukRegistry::CreateNewId(
    const std::string& name, const DukRegistry::CreationCallback& fv)
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
DukRegistry::EntityView(const std::vector<ComponentId>& types)
{
  return reg->View(types);
}

sol::table
DukRegistry::GetProperty(EntityId ent, ComponentId comp)
{
  ASSERT(scriptComponents.find(comp) != scriptComponents.end());
  auto c = reg->GetComponent(ent, comp);
  if(c == nullptr)
  {
    return sol::table{};
  }
  else
  {
    return static_cast<ScriptComponent*>(c.get())->val;
  }
}

ScriptComponent*
GetScriptComponent(
    const DukRegistry::ScriptComponentMap& scriptComponents,
    EntReg*                                reg,
    EntityId                               ent,
    ComponentId                            comp)
{
  ASSERT(scriptComponents.find(comp) != scriptComponents.end());

  auto c = reg->GetComponent(ent, comp);
  if(c == nullptr)
  {
    auto d = std::make_shared<ScriptComponent>();
    reg->AddComponent(ent, comp, d);
    return d.get();
  }
  else
  {
    return static_cast<ScriptComponent*>(c.get());
  }
}

void
DukRegistry::SetProperty(EntityId ent, ComponentId comp, sol::table value)
{
  ScriptComponent* scriptComponent =
      GetScriptComponent(scriptComponents, reg, ent, comp);
  scriptComponent->val = value;
}

sol::table
DukRegistry::CreateComponent(
    ComponentId comp, sol::state* ctx, const CustomArguments& arguments)
{
  auto res = scriptComponents.find(comp);
  ASSERT(res != scriptComponents.end());
  if(res == scriptComponents.end())
  {
    return sol::table{};
  }

  if(res->second == nullptr)
  {
    return sol::table{};
  }

  auto val = res->second(arguments);
  // ASSERT(val.IsValid());
  return val;
}

void
DukRegistry::DestroyEntity(EntityId id)
{
  reg->DestroyEntity(id);
}
