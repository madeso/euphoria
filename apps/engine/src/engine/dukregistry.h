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
    struct custom_arguments
    {
        std::map<std::string, float> numbers;
    };

    struct script_registry
    {
    public:
        script_registry(core::ecs::registry* r, engine::components* c);

        using creation_callback = sol::protected_function;

        core::ecs::component_id
        create_new_id(const std::string& name, const creation_callback& fv);

        core::ecs::component_id
        create_new_id(const std::string& name);

        core::result<core::ecs::component_id>
        get_custom_component_by_name(const std::string& name);

        std::vector<core::ecs::entity_id>
        entity_view(const std::vector<core::ecs::component_id>& types);

        sol::table
        get_property(core::ecs::entity_id ent, core::ecs::component_id comp);

        void
        set_property
        (
            core::ecs::entity_id ent,
            core::ecs::component_id comp,
            sol::table value
        );

        sol::table
        create_component
        (
            core::ecs::component_id comp,
            Sol* ctx,
            const custom_arguments& arguments
        );

        void
        destroy_entity(core::ecs::entity_id id);

        template <typename T>
        T*
        get_component_or_null(core::ecs::entity_id ent, core::ecs::component_id comp)
        {
            return reg->get_component_or_null<T>(ent, comp);
        }

        using script_component_map = std::map<core::ecs::component_id, creation_callback>;

        core::ecs::registry* reg;
        engine::components* components;
        script_component_map scriptComponents;
    };
}

TYPEID_SETUP_TYPE(euphoria::engine::custom_arguments);

