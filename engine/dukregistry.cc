#include "engine/dukregistry.h"

#include <algorithm>

namespace euphoria::engine
{
    struct ScriptComponent : public core::ecs::Component
    {
        COMPONENT_CONSTRUCTOR_DEFINITION(ScriptComponent)

        duk::ObjectReference val;
    };

    COMPONENT_CONSTRUCTOR_IMPLEMENTATION(ScriptComponent)

    DukRegistry::DukRegistry(core::ecs::EntReg* r, Components* c)
        : reg(r), components(c)
    {}

    core::ecs::ComponentId
    DukRegistry::CreateNewId(
            const std::string&            name,
            const duk::FunctionReference& fv)
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

    duk::ObjectReference
    DukRegistry::GetProperty(
            core::ecs::EntityId    ent,
            core::ecs::ComponentId comp)
    {
        ASSERT(scriptComponents.find(comp) != scriptComponents.end());
        auto c = reg->GetComponent(ent, comp);
        if (c == nullptr)
        {
            return duk::ObjectReference {};
        }
        else
        {
            return static_cast<ScriptComponent*>(c.get())->val;
        }
    }

    ScriptComponent*
    GetScriptComponent(
            const DukRegistry::ScriptComponentMap& scriptComponents,
            core::ecs::EntReg*                     reg,
            core::ecs::EntityId                    ent,
            core::ecs::ComponentId                 comp)
    {
        ASSERT(scriptComponents.find(comp) != scriptComponents.end());

        auto c = reg->GetComponent(ent, comp);
        if (c == nullptr)
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
            duk::ObjectReference   value)
    {
        ScriptComponent* scriptComponent
                = GetScriptComponent(scriptComponents, reg, ent, comp);
        scriptComponent->val = value;
    }

    duk::ObjectReference
    DukRegistry::CreateComponent(
            core::ecs::ComponentId comp,
            duk::Context*          ctx,
            const CustomArguments& arguments)
    {
        auto res = scriptComponents.find(comp);
        ASSERT(res != scriptComponents.end());
        if (res == scriptComponents.end())
        {
            return duk::ObjectReference {};
        }

        if (!res->second.IsValid())
        {
            return duk::ObjectReference {};
        }

        auto val = res->second.Call<duk::ObjectReference>(ctx, arguments);
        ASSERT(val.IsValid());
        // todo: need to increase refcount on val here like functions, right?
        return val;
    }

    void
    DukRegistry::DestroyEntity(core::ecs::EntityId id)
    {
        reg->DestroyEntity(id);
    }
}  // namespace euphoria::engine

TYPEID_SETUP_TYPE(euphoria::engine::ScriptComponent);
