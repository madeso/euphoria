#include "engine/dukregistry.h"

#include <algorithm>

namespace euphoria::engine
{
    struct ScriptComponent : public core::Component
    {
        COMPONENT_CONSTRUCTOR_DEFINITION(ScriptComponent)

        duk::ObjectReference val;
    };

    COMPONENT_CONSTRUCTOR_IMPLEMENTATION(ScriptComponent)

    DukRegistry::DukRegistry(core::EntReg* r, Components* c)
        : reg(r), components(c)
    {}

    core::ComponentId
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
            const std::string& name,
            core::ComponentId* id)
    {
        return reg->GetCustomComponentByName(name, id);
    }

    std::vector<core::EntityId>
    DukRegistry::EntityView(const std::vector<core::ComponentId>& types)
    {
        return reg->View(types);
    }

    duk::ObjectReference
    DukRegistry::GetProperty(core::EntityId ent, core::ComponentId comp)
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
            core::EntReg*                          reg,
            core::EntityId                         ent,
            core::ComponentId                      comp)
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
            core::EntityId       ent,
            core::ComponentId    comp,
            duk::ObjectReference value)
    {
        ScriptComponent* scriptComponent
                = GetScriptComponent(scriptComponents, reg, ent, comp);
        scriptComponent->val = value;
    }

    duk::ObjectReference
    DukRegistry::CreateComponent(
            core::ComponentId      comp,
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
    DukRegistry::DestroyEntity(core::EntityId id)
    {
        reg->DestroyEntity(id);
    }
}  // namespace euphoria::engine

TYPEID_SETUP_TYPE(euphoria::engine::ScriptComponent);
