#include "runner/dukregistry.h"

#include <algorithm>

#include "assert/assert.h"
#include "log/log.h"


namespace eu::runner
{
    struct ScriptComponent
    {
        sol::table val;
    };

    ScriptRegistry::ScriptRegistry(core::ecs::Registry* r, runner::Components* c)
        : reg(r)
        , components(c)
    {
    }

    core::ecs::ComponentIndex
    ScriptRegistry::create_new_id
    (
        const std::string& name,
        const ScriptRegistry::CreationCallback& fv
    )
    {
        const auto id = reg->register_component<ScriptComponent>(name);
        script_components[id] = fv;
        name_to_id[name] = id;
        return id;
    }

    core::ecs::ComponentIndex
    ScriptRegistry::create_new_id
    (
        const std::string& name
    )
    {
        const auto id = reg->register_component<ScriptComponent>(name);
        name_to_id[name] = id;
        return id;
    }

    Result<core::ecs::ComponentIndex>
    ScriptRegistry::get_custom_component_by_name
    (
        const std::string& name
    )
    const
    {
        auto found = name_to_id.find(name);
        if(found != name_to_id.end())
        {
            return Result<core::ecs::ComponentIndex>::create_value(found->second);
        }
        else
        {
            return Result<core::ecs::ComponentIndex>::create_error
            (
                fmt::format("Unable to find {}", name)
            );
        }
    }

    std::vector<core::ecs::EntityHandle>
    ScriptRegistry::entity_view(const std::vector<core::ecs::ComponentIndex>& types) const
    {
        return reg->view(types);
    }

    sol::table
    ScriptRegistry::get_property
    (
        core::ecs::EntityHandle ent,
        core::ecs::ComponentIndex comp
    )
    {
        ASSERT(script_components.find(comp) != script_components.end());
        auto* c = reg->get_component_or_null<ScriptComponent>(ent, comp);
        
        if(c == nullptr)
        {
            return sol::table{};
        }
        else
        {
            return c->val;
        }
    }

    ScriptComponent*
    get_script_component(core::ecs::Registry *reg, core::ecs::EntityHandle ent, core::ecs::ComponentIndex comp)
    {
        // todo(Gustav): add a get_create_function
        auto* c = reg->get_component_or_null<ScriptComponent>(ent, comp);
        if(c == nullptr)
        {
            reg->add_component(ent, comp, ScriptComponent{});
            return reg->get_component_or_null<ScriptComponent>(ent, comp);
        }
        else
        {
            return c;
        }
    }

    void
    ScriptRegistry::set_property
    (
        core::ecs::EntityHandle ent,
        core::ecs::ComponentIndex comp,
        sol::table value
    )
    const
    {
        ScriptComponent* component = get_script_component(reg, ent, comp);
        component->val = value;
    }

    sol::table
    ScriptRegistry::create_component
    (
        core::ecs::ComponentIndex comp,
        LuaState* ctx,
        const CustomArguments& arguments
    )
    {
        const auto name = reg->get_component_debug_name(comp);

        auto res = script_components.find(comp);
        if(res == script_components.end())
        {
            // no custom function, use use a empty table...?
            // or perhaps use null?
            return sol::table{ctx->state, sol::create};
        }

        auto val = res->second(arguments);
        if(val.valid())
        {
            sol::table ret = val;
            return ret;
        }
        else
        {
            const sol::error err = val;
            const auto message = fmt::format("Failed to call create for component {0}({1}): {2}", name, core::ecs::c_comp(comp), err.what());
            LOG_ERROR("{0}", message);
            if(!ctx->has_error)
            {
                ctx->has_error = true;
                ctx->error = message;
            }
            return sol::table{ctx->state, sol::create};
        }
    }

    void
    ScriptRegistry::destroy_entity(core::ecs::EntityHandle id) const
    {
        reg->destroy(id);
    }
}
