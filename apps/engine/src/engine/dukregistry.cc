#include "engine/dukregistry.h"

#include <algorithm>

#include "core/assert.h"
#include "core/log.h"

LOG_SPECIFY_DEFAULT_LOGGER("engine.duk.registry")

namespace euphoria::engine
{
    struct ScriptComponent : public core::ecs::Component
    {
        COMPONENT_CONSTRUCTOR_DEFINITION(ScriptComponent)

        sol::table val;
    };

    COMPONENT_CONSTRUCTOR_IMPLEMENTATION(ScriptComponent)

    DukRegistry::DukRegistry(core::ecs::Registry* r, Components* c)
        : reg(r), components(c)
    {}

    core::ecs::ComponentId
    DukRegistry::CreateNewId(
            const std::string&            name,
            const DukRegistry::CreationCallback& fv)
    {
        const auto id        = reg->NewComponentType(name);
        scriptComponents[id] = fv;
        return id;
    }

    bool
    DukRegistry::GetCustomComponentByName(
            const std::string&      name,
            core::ecs::ComponentId* id)
    {
        return reg->GetCustomComponentByName(name, id);
    }

    std::vector<core::ecs::EntityId>
    DukRegistry::EntityView(const std::vector<core::ecs::ComponentId>& types)
    {
        return reg->View(types);
    }

    sol::table
    DukRegistry::GetProperty(
            core::ecs::EntityId    ent,
            core::ecs::ComponentId comp)
    {
        ASSERT(scriptComponents.find(comp) != scriptComponents.end());
        auto c = reg->GetComponent(ent, comp);
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
            core::ecs::Registry*                     reg,
            core::ecs::EntityId                    ent,
            core::ecs::ComponentId                 comp)
    {
        ASSERT(scriptComponents.find(comp) != scriptComponents.end());

        auto c = reg->GetComponent(ent, comp);
        if(c == nullptr)
        {
            auto d = std::make_shared<ScriptComponent>();
            reg->AddComponent(ent, comp, d);
            return d.get();
        }
        else
        {
            return static_cast<ScriptComponent*>(c.get());
        }
    }

    void
    DukRegistry::SetProperty(
            core::ecs::EntityId    ent,
            core::ecs::ComponentId comp,
            sol::table   value)
    {
        ScriptComponent* scriptComponent = GetScriptComponent(scriptComponents, reg, ent, comp);
        scriptComponent->val = value;
    }

    sol::table
    DukRegistry::CreateComponent(
            core::ecs::ComponentId comp,
            Sol*          ctx,
            const CustomArguments& arguments)
    {
        auto res = scriptComponents.find(comp);
        ASSERT(res != scriptComponents.end());
        if(res == scriptComponents.end())
        {
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
            const auto name = reg->GetComponentName(comp);
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
    DukRegistry::DestroyEntity(core::ecs::EntityId id)
    {
        reg->DestroyEntity(id);
    }
}  // namespace euphoria::engine

TYPEID_SETUP_TYPE(euphoria::engine::ScriptComponent);
