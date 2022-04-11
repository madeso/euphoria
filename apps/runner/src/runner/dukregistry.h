#pragma once

#include "core/ecs.h"
#include "core/result.h"
#include "core/sol.h"

#include "runner/components.h"
#include "runner/ecs.systems.h"

#include <map>
#include <string>
#include <memory>
#include <functional>

namespace euphoria::runner
{
    struct CustomArguments
    {
        std::map<std::string, float> numbers;
    };

    struct ScriptRegistry
    {
        ScriptRegistry(core::ecs::Registry* r, runner::Components* c);

        using CreationCallback = sol::protected_function;

        core::ecs::ComponentIndex
        create_new_id(const std::string& name, const CreationCallback& fv);

        [[nodiscard]] core::ecs::ComponentIndex
        create_new_id(const std::string& name);

        [[nodiscard]] core::Result<core::ecs::ComponentIndex>
        get_custom_component_by_name(const std::string& name) const;

        [[nodiscard]] std::vector<core::ecs::EntityHandle>
        entity_view(const std::vector<core::ecs::ComponentIndex>& types) const;

        sol::table
        get_property(core::ecs::EntityHandle ent, core::ecs::ComponentIndex comp);

        void
        set_property
        (
            core::ecs::EntityHandle ent,
            core::ecs::ComponentIndex comp,
            sol::table value
        ) const;

        sol::table
        create_component
        (
            core::ecs::ComponentIndex comp,
            LuaState* ctx,
            const CustomArguments& arguments
        );

        void
        destroy_entity(core::ecs::EntityHandle id) const;

        template <typename T>
        T*
        get_component_or_null(core::ecs::EntityHandle ent, core::ecs::ComponentIndex comp)
        {
            return reg->get_component_or_null<T>(ent, comp);
        }

        using ScriptComponentMap = std::map<core::ecs::ComponentIndex, CreationCallback>;
        using NameToIdMap = std::map<std::string, core::ecs::ComponentIndex>;

        core::ecs::Registry* reg;
        runner::Components* components;
        ScriptComponentMap script_components;
        NameToIdMap name_to_id;
    };
}

TYPEID_SETUP_TYPE(euphoria::runner::CustomArguments);

