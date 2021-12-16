#pragma once

#include "core/ecs-systems.h"
#include "core/ecs.h"
#include "core/result.h"
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

    struct ScriptRegistry
    {
        ScriptRegistry(core::ecs::Registry* r, engine::Components* c);

        using CreationCallback = sol::protected_function;

        core::ecs::ComponentId
        create_new_id(const std::string& name, const CreationCallback& fv);

        [[nodiscard]] core::ecs::ComponentId
        create_new_id(const std::string& name) const;

        [[nodiscard]] core::Result<core::ecs::ComponentId>
        get_custom_component_by_name(const std::string& name) const;

        [[nodiscard]] std::vector<core::ecs::EntityId>
        entity_view(const std::vector<core::ecs::ComponentId>& types) const;

        sol::table
        get_property(core::ecs::EntityId ent, core::ecs::ComponentId comp);

        void
        set_property
        (
            core::ecs::EntityId ent,
            core::ecs::ComponentId comp,
            sol::table value
        ) const;

        sol::table
        create_component
        (
                core::ecs::ComponentId comp,
                LuaState* ctx,
                const CustomArguments& arguments
        );

        void
        destroy_entity(core::ecs::EntityId id) const;

        template <typename T>
        T*
        get_component_or_null(core::ecs::EntityId ent, core::ecs::ComponentId comp)
        {
            return reg->get_component_or_null<T>(ent, comp);
        }

        using ScriptComponentMap = std::map<core::ecs::ComponentId, CreationCallback>;

        core::ecs::Registry* reg;
        engine::Components* components;
        ScriptComponentMap script_components;
    };
}

TYPEID_SETUP_TYPE(euphoria::engine::CustomArguments);

