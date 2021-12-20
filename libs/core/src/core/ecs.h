#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <vector>

#include "core/typeinfo.h"
#include "core/ecs.id.h"
#include "core/assert.h"
#include "core/result.h"

#include "euph_generated_config.h"

#if BUILD_ENTITY_DEBUG_COMPONENT == 1
#define COMPONENT_CONSTRUCTOR_IMPLEMENTATION(X) \
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
    get_value(EntityId id);

    EntityVersion
    get_version(EntityVersion id);

    EntityId
    get_id(EntityId id, EntityVersion version);

    ////////////////////////////////////////////////////////////////////////////////

    struct Component
    {
#if BUILD_ENTITY_DEBUG_COMPONENT == 1
        component(TypeName n, TypeId i);
#else
        Component() = default;
#endif
        virtual ~Component() = default;

        Component(const Component&) = delete;
        Component(Component&&) = delete;
        void operator=(const Component&) = delete;
        void operator=(Component&&) = delete;

        // todo(Gustav): change to a virtual function and a hashed string struct
#if BUILD_ENTITY_DEBUG_COMPONENT == 1
        const TypeName type_name;
        const TypeId type_id;
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
        on_created(EntityId ent) = 0;
    };

    template <typename Func>
    struct RegistryEntityCallbackFunction : public RegistryEntityCallback
    {
        Func func;

        explicit RegistryEntityCallbackFunction(Func f) : func(f) {}

        void
        on_created(EntityId ent) override
        {
            func(ent);
        }
    };

    template <typename Func>
    std::shared_ptr<RegistryEntityCallback>
    make_registry_entity_callback(Func f)
    {
        return std::make_shared<RegistryEntityCallbackFunction<Func>>(f);
    }

    ////////////////////////////////////////////////////////////////////////////////

    struct RegistryImplementation;
    struct Registry
    {
        Registry();
        ~Registry();

        Registry(const Registry&) = delete;
        Registry(Registry&&) = delete;
        void operator=(const Registry&) = delete;
        void operator=(Registry&&) = delete;

        EntityId
        create_new_entity();

        void
        post_create(EntityId id);

        void
        add_callback(std::shared_ptr<RegistryEntityCallback> callback);

        [[nodiscard]] bool
        is_alive(EntityId id) const;

        void
        destroy_entity(EntityId id);


        ComponentId
        register_new_component_type(const std::string& name);

        [[nodiscard]] std::string
        get_component_name(ComponentId id) const;

        Result<ComponentId>
        get_custom_component_by_name(const std::string& name);

        std::shared_ptr<Component>
        get_component(EntityId entity, ComponentId component);

        void
        add_component_to_entity
        (
            EntityId entity,
            ComponentId comp,
            std::shared_ptr<Component> data
        );


        std::vector<EntityId>
        get_entities_with_components(const std::vector<ComponentId>& components);

        template <typename T>
        T*
        get_component_or_null(EntityId entity, ComponentId component)
        {
            auto c = get_component(entity, component);
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
        remove_entities_tagged_for_removal();

    private:
        std::unique_ptr<RegistryImplementation> impl;
    };

    ////////////////////////////////////////////////////////////////////////////////

}
