#include "engine/dukregistry.h"

#include <algorithm>

#include "core/assert.h"
#include "core/log.h"

LOG_SPECIFY_DEFAULT_LOGGER("engine.duk.registry")

namespace euphoria::engine
{
    struct ScriptComponent : public core::ecs::component
    {
        COMPONENT_CONSTRUCTOR_DEFINITION(ScriptComponent)

        sol::table val;
    };

    COMPONENT_CONSTRUCTOR_IMPLEMENTATION(ScriptComponent)

    DukRegistry::DukRegistry(core::ecs::registry* r, Components* c)
        : reg(r), components(c)
    {}

    core::ecs::component_id
    DukRegistry::CreateNewId(
            const std::string&            name,
            const DukRegistry::CreationCallback& fv)
    {
        const auto id        = reg->register_new_component_type(name);
        scriptComponents[id] = fv;
        return id;
    }

    core::ecs::component_id
        DukRegistry::CreateNewId(
            const std::string& name)
    {
        const auto id = reg->register_new_component_type(name);
        return id;
    }

    bool
    DukRegistry::GetCustomComponentByName(
            const std::string&      name,
            core::ecs::component_id* id)
    {
        return reg->get_custom_component_by_name(name, id);
    }

    std::vector<core::ecs::entity_id>
    DukRegistry::EntityView(const std::vector<core::ecs::component_id>& types)
    {
        return reg->get_entities_with_components(types);
    }

    sol::table
    DukRegistry::GetProperty(
            core::ecs::entity_id    ent,
            core::ecs::component_id comp)
    {
        ASSERT(scriptComponents.find(comp) != scriptComponents.end());
        auto c = reg->get_component(ent, comp);
        if(c == nullptr)
        {
            return sol::table{};
        }
        else
        {
            return static_cast<ScriptComponent*>(c.get())->val;
        }
    }

    ScriptComponent*
    GetScriptComponent(
            const DukRegistry::ScriptComponentMap& scriptComponents,
            core::ecs::registry*                     reg,
            core::ecs::entity_id                    ent,
            core::ecs::component_id                 comp)
    {
        // ASSERT(scriptComponents.find(comp) != scriptComponents.end());

        auto c = reg->get_component(ent, comp);
        if(c == nullptr)
        {
            auto d = std::make_shared<ScriptComponent>();
            reg->add_component_to_entity(ent, comp, d);
            return d.get();
        }
        else
        {
            return static_cast<ScriptComponent*>(c.get());
        }
    }

    void
    DukRegistry::SetProperty(
            core::ecs::entity_id    ent,
            core::ecs::component_id comp,
            sol::table   value)
    {
        ScriptComponent* scriptComponent = GetScriptComponent(scriptComponents, reg, ent, comp);
        scriptComponent->val = value;
    }

    sol::table
    DukRegistry::CreateComponent(
            core::ecs::component_id comp,
            Sol*          ctx,
            const CustomArguments& arguments)
    {
        const auto name = reg->get_component_name(comp);

        auto res = scriptComponents.find(comp);
        if(res == scriptComponents.end())
        {
            // no custom function, use use a empty table...?
            // or perhaps use null?
            return sol::table{ctx->lua, sol::create};
        }

        auto val = res->second(arguments);
        if(val.valid())
        {
            sol::table ret = val;
            return ret;
        }
        else
        {
            sol::error err = val;
            const auto message = fmt::format("Failed to call create for component {0}({1}): {2}", name, comp, err.what());
            LOG_ERROR("{0}", message);
            if(!ctx->has_error)
            {
                ctx->has_error = true;
                ctx->error = message;
            }
            return sol::table{ctx->lua, sol::create};
        }
    }

    void
    DukRegistry::DestroyEntity(core::ecs::entity_id id)
    {
        reg->destroy_entity(id);
    }
}  // namespace euphoria::engine

TYPEID_SETUP_TYPE(euphoria::engine::ScriptComponent);
