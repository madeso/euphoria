#pragma once

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
    entity_id
    get_value(entity_id id);

    entity_version
    get_version(entity_version id);

    entity_id
    get_id(entity_id id, entity_version version);

    ////////////////////////////////////////////////////////////////////////////////

    struct component
    {
#if BUILD_ENTITY_DEBUG_COMPONENT == 1
        component(TypeName n, TypeId i);
#else
        component() = default;
#endif
        virtual ~component() = default;

        component(const component&) = delete;
        component(component&&) = delete;
        void operator=(const component&) = delete;
        void operator=(component&&) = delete;

        // todo(Gustav): change to a virtual function and a hashed string struct
#if BUILD_ENTITY_DEBUG_COMPONENT == 1
        const TypeName type_name;
        const TypeId   type_id;
#endif
    };

    ////////////////////////////////////////////////////////////////////////////////

    struct registry_entity_callback
    {
        registry_entity_callback() = default;
        virtual ~registry_entity_callback() = default;

        registry_entity_callback(const registry_entity_callback&) = delete;
        registry_entity_callback(registry_entity_callback&&) = delete;
        void operator=(const registry_entity_callback&) = delete;
        void operator=(registry_entity_callback&&) = delete;

        virtual void
        on_created(entity_id ent) = 0;
    };

    template <typename Func>
    struct registry_entity_callback_function : public registry_entity_callback
    {
        Func func;

        explicit registry_entity_callback_function(Func f) : func(f) {}

        void
        on_created(entity_id ent) override
        {
            func(ent);
        }
    };

    template <typename Func>
    std::shared_ptr<registry_entity_callback>
    make_registry_entity_callback(Func f)
    {
        return std::make_shared<registry_entity_callback_function<Func>>(f);
    }

    ////////////////////////////////////////////////////////////////////////////////

    struct registry_impl;
    struct registry
    {
        registry();
        ~registry();

        registry(const registry&) = delete;
        registry(registry&&) = delete;
        void operator=(const registry&) = delete;
        void operator=(registry&&) = delete;

        entity_id
        create_new_entity();

        void
        post_create(entity_id id);

        void
        add_callback(std::shared_ptr<registry_entity_callback> callback);

        [[nodiscard]] bool
        is_alive(entity_id id) const;

        void
        destroy_entity(entity_id id);


        component_id
        register_new_component_type(const std::string& name);

        std::string
        get_component_name(component_id id) const;

        // todo(Gustav): make it return a std::optional instead
        bool
        get_custom_component_by_name(const std::string& name, component_id* id);

        std::shared_ptr<component>
        get_component(entity_id entity, component_id component);

        void
        add_component_to_entity
        (
            entity_id                   entity,
            component_id                comp,
            std::shared_ptr<component> data
        );


        std::vector<entity_id>
        get_entities_with_components(const std::vector<component_id>& components);

        template <typename T>
        T*
        get_component_or_null(entity_id entity, component_id component)
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
        std::unique_ptr<registry_impl> impl;
    };

    ////////////////////////////////////////////////////////////////////////////////

}
