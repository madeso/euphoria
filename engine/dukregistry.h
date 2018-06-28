#ifndef EUPHORIA_DUKREGISTRY_H
#define EUPHORIA_DUKREGISTRY_H

#include "core/componentsystem.h"
#include "core/duk.h"
#include "core/entity.h"

#include "engine/components.h"

#include <map>
#include <memory>

class DukRegistry
{
 public:
  DukRegistry(EntReg* r, Components* components);

  ComponentId
  CreateNewId(const std::string& name, const FunctionVar& fv);

  bool
  GetCustomComponentByName(const std::string& name, ComponentId* id);

  std::vector<EntityId>
  EntityView(const std::vector<ComponentId>& types);

  DukValue
  GetProperty(EntityId ent, ComponentId comp);

  void
  SetProperty(EntityId ent, ComponentId comp, DukValue value);

  DukValue
  CreateComponent(ComponentId comp, Context* ctx);

  void
  DestroyEntity(EntityId id);

  template <typename T>
  T*
  GetComponentOrNull(EntityId ent, ComponentId comp)
  {
    return reg->GetComponentOrNull<T>(ent, comp);
  }

  using ScriptComponentMap = std::map<ComponentId, FunctionVar>;

  EntReg*            reg;
  Components*        components;
  ScriptComponentMap scriptComponents;
};


#endif  // EUPHORIA_DUKREGISTRY_H
