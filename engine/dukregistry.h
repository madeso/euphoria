#ifndef EUPHORIA_DUKREGISTRY_H
#define EUPHORIA_DUKREGISTRY_H

#include "core/ecs-systems.h"
#include "core/ecs.h"

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
        DukRegistry(core::ecs::EntReg* r, Components* components);

        core::ecs::ComponentId
        CreateNewId(const std::string& name, const duk::FunctionReference& fv);

        bool
        GetCustomComponentByName(
                const std::string&      name,
                core::ecs::ComponentId* id);

        std::vector<core::ecs::EntityId>
        EntityView(const std::vector<core::ecs::ComponentId>& types);

        duk::ObjectReference
        GetProperty(core::ecs::EntityId ent, core::ecs::ComponentId comp);

        void
        SetProperty(
                core::ecs::EntityId    ent,
                core::ecs::ComponentId comp,
                duk::ObjectReference   value);

        duk::ObjectReference
        CreateComponent(
                core::ecs::ComponentId comp,
                duk::Context*          ctx,
                const CustomArguments& arguments);

        void
        DestroyEntity(core::ecs::EntityId id);

        template <typename T>
        T*
        GetComponentOrNull(core::ecs::EntityId ent, core::ecs::ComponentId comp)
        {
            return reg->GetComponentOrNull<T>(ent, comp);
        }

        using ScriptComponentMap
                = std::map<core::ecs::ComponentId, duk::FunctionReference>;

        core::ecs::EntReg* reg;
        Components*        components;
        ScriptComponentMap scriptComponents;
    };
}  // namespace euphoria::engine

TYPEID_SETUP_TYPE(euphoria::engine::CustomArguments);

#endif  // EUPHORIA_DUKREGISTRY_H
