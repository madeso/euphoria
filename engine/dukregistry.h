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
  getPosition2dId();

  ComponentId
  getSpriteId();

  ComponentId
  CreateNewId(const std::string& name);

  bool
  GetCustomComponentByName(const std::string& name, ComponentId* id);

  std::vector<EntityId>
  entities(const std::vector<ComponentId>& types);

  DukValue
  GetProperty(EntityId ent, ComponentId comp);

  void
  SetProperty(EntityId ent, ComponentId comp, DukValue value);

  template <typename T>
  T*
  GetComponentOrNull(EntityId ent, ComponentId comp)
  {
    return reg->GetComponentOrNull<T>(ent, comp);
  }

 private:
  EntReg*                  reg;
  Components*              components;
  std::vector<ComponentId> scriptComponents;
};


#endif  // EUPHORIA_DUKREGISTRY_H
