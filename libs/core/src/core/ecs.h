#ifndef EUPHORIA_ENTITY_H
#define EUPHORIA_ENTITY_H

#include <cstdint>
#include <string>
#include <memory>
#include <vector>

#include "core/typeinfo.h"
#include "core/ecs-id.h"
#include "core/assert.h"

#include "euph_generated_config.h"

#if BUILD_ENTITY_DEBUG_COMPONENT == 1
#define COMPONENT_CONSTRUCTOR_IMPLEMENTATION(X)                                \
    X::X() : Component(TYPEID_NAME(X), TYPEID_ID(X)) {}
#define COMPONENT_CONSTRUCTOR_ARG(X) Component(TYPEID_NAME(X), TYPEID_ID(X)),
#define COMPONENT_CONSTRUCTOR_DEFINITION(X) X();
#else
#define COMPONENT_CONSTRUCTOR_IMPLEMENTATION(X)
#define COMPONENT_CONSTRUCTOR_DEFINITION(X)
#define COMPONENT_CONSTRUCTOR_ARG(X)
#endif

////////////////////////////////////////////////////////////////////////////////

namespace euphoria::core::ecs
{
    EntityId
    GetValue(EntityId id);

    EntityVersion
    GetVersion(EntityVersion id);

    EntityId
    GetId(EntityId id, EntityVersion version);

    ////////////////////////////////////////////////////////////////////////////////

    struct Component
    {
#if BUILD_ENTITY_DEBUG_COMPONENT == 1
        Component(TypeName n, TypeId i);
#else
        Component() = default;
#endif
        virtual ~Component() = default;

        Component(const Component&) = delete;
        Component(Component&&) = delete;
        void operator=(const Component&) = delete;
        void operator=(Component&&) = delete;

#if BUILD_ENTITY_DEBUG_COMPONENT == 1
        const TypeName type_name;
        const TypeId   type_id;
#endif
    };

    ////////////////////////////////////////////////////////////////////////////////

    struct RegistryEntityCallback
    {
        RegistryEntityCallback() = default;
        virtual ~RegistryEntityCallback() = default;

        RegistryEntityCallback(const RegistryEntityCallback&) = delete;
        RegistryEntityCallback(RegistryEntityCallback&&) = delete;
        void operator=(const RegistryEntityCallback&) = delete;
        void operator=(RegistryEntityCallback&&) = delete;

        virtual void
        OnCreated(EntityId ent) = 0;
    };

    template <typename Func>
    struct RegistryEntityCallbackFunction : public RegistryEntityCallback
    {
        Func func;

        explicit RegistryEntityCallbackFunction(Func f) : func(f) {}

        void
        OnCreated(EntityId ent) override
        {
            func(ent);
        }
    };

    template <typename Func>
    std::shared_ptr<RegistryEntityCallback>
    MakeRegistryEntityCallback(Func f)
    {
        return std::make_shared<RegistryEntityCallbackFunction<Func>>(f);
    }

    ////////////////////////////////////////////////////////////////////////////////

    struct RegistryImpl;
    struct Registry
    {
        Registry();
        ~Registry();

        Registry(const Registry&) = delete;
        Registry(Registry&&) = delete;
        void operator=(const Registry&) = delete;
        void operator=(Registry&&) = delete;

        EntityId
        Create();

        void
        PostCreate(EntityId id);

        void
        AddCallback(std::shared_ptr<RegistryEntityCallback> callback);

        [[nodiscard]] bool
        IsAlive(EntityId id) const;

        void
        DestroyEntity(EntityId id);


        ComponentId
        NewComponentType(const std::string& name);

        std::string
        GetComponentName(ComponentId id) const;

        // todo(Gustav): make it return a std::optional instead
        bool
        GetCustomComponentByName(const std::string& name, ComponentId* id);

        std::shared_ptr<Component>
        GetComponent(EntityId entity, ComponentId component);

        void
        AddComponent(
                EntityId                   entity,
                ComponentId                component,
                std::shared_ptr<Component> data);


        std::vector<EntityId>
        View(const std::vector<ComponentId>& components);

        template <typename T>
        T*
        GetComponentOrNull(EntityId entity, ComponentId component)
        {
            auto c = GetComponent(entity, component);
            if(c == nullptr)
            {
                return nullptr;
            }
            else
            {
#if BUILD_ENTITY_DEBUG_COMPONENT == 1
                ASSERTX(c->type_id == TYPEID_ID(T),
                        c->type_name,
                        TYPEID_NAME(T));
#endif
                return static_cast<T*>(c.get());
            }
        }

        void
        AddAdded();

        void
        RemoveRemoved();

    private:
        std::unique_ptr<RegistryImpl> impl;
    };

    ////////////////////////////////////////////////////////////////////////////////

}  // namespace euphoria::core::ecs

#endif  // EUPHORIA_ENTITY_H
