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
        DukRegistry(core::ecs::registry* r, Components* components);

        using CreationCallback = sol::protected_function;

        core::ecs::component_id
        CreateNewId(const std::string& name, const CreationCallback& fv);

        core::ecs::component_id
        CreateNewId(const std::string& name);

        bool
        GetCustomComponentByName(const std::string& name, core::ecs::component_id* id);

        std::vector<core::ecs::entity_id>
        EntityView(const std::vector<core::ecs::component_id>& types);

        sol::table
        GetProperty(core::ecs::entity_id ent, core::ecs::component_id comp);

        void
        SetProperty(
                core::ecs::entity_id    ent,
                core::ecs::component_id comp,
                sol::table   value);

        sol::table
        CreateComponent(
                core::ecs::component_id comp,
                Sol*          ctx,
                const CustomArguments& arguments);

        void
        DestroyEntity(core::ecs::entity_id id);

        template <typename T>
        T*
        GetComponentOrNull(core::ecs::entity_id ent, core::ecs::component_id comp)
        {
            return reg->get_component_or_null<T>(ent, comp);
        }

        using ScriptComponentMap = std::map<core::ecs::component_id, CreationCallback>;

        core::ecs::registry* reg;
        Components*        components;
        ScriptComponentMap scriptComponents;
    };
}  // namespace euphoria::engine

TYPEID_SETUP_TYPE(euphoria::engine::CustomArguments);

#endif  // EUPHORIA_DUKREGISTRY_H
