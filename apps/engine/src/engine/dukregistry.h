#ifndef EUPHORIA_DUKREGISTRY_H
#define EUPHORIA_DUKREGISTRY_H

#include "core/ecs-systems.h"
#include "core/ecs.h"

#include "core/sol.h"

#include "engine/components.h"

#include <map>
#include <string>
#include <memory>
#include <functional>

namespace euphoria::engine
{
    struct CustomArguments
    {
        std::map<std::string, float> numbers;
    };

    struct DukRegistry
    {
    public:
        DukRegistry(core::ecs::Registry* r, Components* components);

        using CreationCallback = sol::protected_function;

        core::ecs::ComponentId
        CreateNewId(const std::string& name, const CreationCallback& fv);

        core::ecs::ComponentId
        CreateNewId(const std::string& name);

        bool
        GetCustomComponentByName(const std::string& name, core::ecs::ComponentId* id);

        std::vector<core::ecs::EntityId>
        EntityView(const std::vector<core::ecs::ComponentId>& types);

        sol::table
        GetProperty(core::ecs::EntityId ent, core::ecs::ComponentId comp);

        void
        SetProperty(
                core::ecs::EntityId    ent,
                core::ecs::ComponentId comp,
                sol::table   value);

        sol::table
        CreateComponent(
                core::ecs::ComponentId comp,
                Sol*          ctx,
                const CustomArguments& arguments);

        void
        DestroyEntity(core::ecs::EntityId id);

        template <typename T>
        T*
        GetComponentOrNull(core::ecs::EntityId ent, core::ecs::ComponentId comp)
        {
            return reg->GetComponentOrNull<T>(ent, comp);
        }

        using ScriptComponentMap = std::map<core::ecs::ComponentId, CreationCallback>;

        core::ecs::Registry* reg;
        Components*        components;
        ScriptComponentMap scriptComponents;
    };
}  // namespace euphoria::engine

TYPEID_SETUP_TYPE(euphoria::engine::CustomArguments);

#endif  // EUPHORIA_DUKREGISTRY_H
