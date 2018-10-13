#ifndef EUPHORIA_DUKREGISTRY_H
#define EUPHORIA_DUKREGISTRY_H

#include "core/componentsystem.h"
#include "core/entity.h"

#include "duk/functionreference.h"

#include "engine/components.h"

#include <map>
#include <string>
#include <memory>

struct CustomArguments
{
  std::map<std::string, float> numbers;
};

class DukRegistry
{
 public:
  DukRegistry(EntReg* r, Components* components);

  ComponentId
  CreateNewId(const std::string& name, const duk::FunctionReference& fv);

  bool
  GetCustomComponentByName(const std::string& name, ComponentId* id);

  std::vector<EntityId>
  EntityView(const std::vector<ComponentId>& types);

  duk::ObjectReference
  GetProperty(EntityId ent, ComponentId comp);

  void
  SetProperty(EntityId ent, ComponentId comp, duk::ObjectReference value);

  duk::ObjectReference
  CreateComponent(
      ComponentId comp, duk::Context* ctx, const CustomArguments& arguments);

  void
  DestroyEntity(EntityId id);

  template <typename T>
  T*
  GetComponentOrNull(EntityId ent, ComponentId comp)
  {
    return reg->GetComponentOrNull<T>(ent, comp);
  }

  using ScriptComponentMap = std::map<ComponentId, duk::FunctionReference>;

  EntReg*            reg;
  Components*        components;
  ScriptComponentMap scriptComponents;
};


#endif  // EUPHORIA_DUKREGISTRY_H
