#ifndef EUPHORIA_DUKREGISTRY_H
#define EUPHORIA_DUKREGISTRY_H

#include "core/componentsystem.h"
#include "core/entity.h"

#include "core/sol.h"

#include "engine/components.h"

#include <map>
#include <string>
#include <memory>
#include <functional>

struct CustomArguments
{
  std::map<std::string, float> numbers;
};
TYPEID_SETUP_TYPE(CustomArguments);

class DukRegistry
{
 public:
  DukRegistry(EntReg* r, Components* components);

  using CreationCallback = std::function<sol::table(const CustomArguments&)>;

  ComponentId
  CreateNewId(const std::string& name, const CreationCallback& fv);

  bool
  GetCustomComponentByName(const std::string& name, ComponentId* id);

  std::vector<EntityId>
  EntityView(const std::vector<ComponentId>& types);

  sol::table
  GetProperty(EntityId ent, ComponentId comp);

  void
  SetProperty(EntityId ent, ComponentId comp, sol::table value);

  sol::table
  CreateComponent(
      ComponentId comp, sol::state* ctx, const CustomArguments& arguments);

  void
  DestroyEntity(EntityId id);

  template <typename T>
  T*
  GetComponentOrNull(EntityId ent, ComponentId comp)
  {
    return reg->GetComponentOrNull<T>(ent, comp);
  }

  using ScriptComponentMap = std::map<ComponentId, CreationCallback>;

  EntReg*            reg;
  Components*        components;
  ScriptComponentMap scriptComponents;
};


#endif  // EUPHORIA_DUKREGISTRY_H
