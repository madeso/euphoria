#ifndef EUPHORIA_DUKREGISTRY_H
#define EUPHORIA_DUKREGISTRY_H

#include "core/componentsystem.h"
#include "core/entity.h"

#include "duk/functionreference.h"

#include "engine/components.h"

#include <map>
#include <string>
#include <memory>

namespace euphoria::engine
{
  struct CustomArguments
  {
    std::map<std::string, float> numbers;
  };

  class DukRegistry
  {
  public:
    DukRegistry(core::EntReg* r, Components* components);

    core::ComponentId
    CreateNewId(const std::string& name, const duk::FunctionReference& fv);

    bool
    GetCustomComponentByName(const std::string& name, core::ComponentId* id);

    std::vector<core::EntityId>
    EntityView(const std::vector<core::ComponentId>& types);

    duk::ObjectReference
    GetProperty(core::EntityId ent, core::ComponentId comp);

    void
    SetProperty(core::EntityId ent, core::ComponentId comp, duk::ObjectReference value);

    duk::ObjectReference
    CreateComponent(
        core::ComponentId comp, duk::Context* ctx, const CustomArguments& arguments);

    void
    DestroyEntity(core::EntityId id);

    template <typename T>
    T*
    GetComponentOrNull(core::EntityId ent, core::ComponentId comp)
    {
      return reg->GetComponentOrNull<T>(ent, comp);
    }

    using ScriptComponentMap = std::map<core::ComponentId, duk::FunctionReference>;

    core::EntReg*            reg;
    Components*        components;
    ScriptComponentMap scriptComponents;
  };
}

TYPEID_SETUP_TYPE(euphoria::engine::CustomArguments);

#endif  // EUPHORIA_DUKREGISTRY_H
