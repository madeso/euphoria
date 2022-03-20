#pragma once

#include <vector>
#include <optional>
#include <memory>

#include "assert/assert.h"

#include "core/ints.h"

namespace euphoria::core::ecs2
{
    // todo(Gustav): add versioning
    enum EntityHandle : U64 {};

    // todo(Gustav): move detail to detail namespace
    std::size_t c_ent(EntityHandle v);


    enum ComponentIndex : U8 {};

    struct ComponentArrayBase
    {
        virtual ~ComponentArrayBase() = default;
        virtual void remove(EntityHandle) = 0;
    };

    // contains a list of components for a single component type
    template<typename T>
    struct GenericComponentArray : public ComponentArrayBase
    {
        // todo(Gustav): implement better sparse vector
        std::vector<std::optional<T>> components;

        void
        add(EntityHandle entity, T&& component)
        {
            const auto index = c_ent(entity);
            if(components.size() < index + 1)
            {
                components.resize(index + 1);
            }
            ASSERT(components[index].has_value() == false);
            components[index] = std::move(component);
        }


        void
        remove(EntityHandle entity) override
        {
            const auto index = c_ent(entity);
            if(components.size() < index + 1)
            {
                return;
            }

            ASSERT(components[index].has_value());
            components[index] = std::nullopt;
        }


        T&
        get(EntityHandle entity)
        {
            const auto index = c_ent(entity);
            ASSERT(components.size() >= index);
            ASSERT(components[index].has_value());
            return *components[index];
        }
    };


    ///////////////////////////////////////////////////////////////////////////


    struct RegistryPimpl;


    struct Registry
    {
        Registry();

        ~Registry();

        [[nodiscard]] int
        get_number_of_active_entities() const;

        EntityHandle
        create();

        void
        destroy(EntityHandle entity);

        ComponentIndex
        set_component_array(const std::string& name, std::unique_ptr<ComponentArrayBase>&& components);

        std::vector<EntityHandle>
        view(const std::vector<ComponentIndex>& matching_components) const;

        template<typename T>
        ComponentIndex
        register_component(const std::string& name)
        {
            return set_component_array(name, std::make_unique<GenericComponentArray<T>>());
        }


        template<typename T>
        void
        add_component(EntityHandle entity, ComponentIndex comp_ind, T&& component)
        {
            get_components<T>(comp_ind)->add(entity, std::move(component));
            on_added_component(entity, comp_ind);
        }


        template<typename T>
        void
        remove_component(EntityHandle entity, ComponentIndex comp_ind)
        {
            on_removed_component(entity, comp_ind);
            get_components<T>(comp_ind)->remove(entity);
        }


        template<typename T>
        T&
        get_component(EntityHandle entity, ComponentIndex comp_ind)
        {
            return get_components<T>(comp_ind)->get(entity);
        }


        // detail
    private:
        template<typename T>
        GenericComponentArray<T>*
        get_components(ComponentIndex comp_ind)
        {
            // todo(Gustav): add validation here to the Signature...?
            return static_cast<GenericComponentArray<T>*>(get_components_base(comp_ind));
        }

        ComponentArrayBase*
        get_components_base(ComponentIndex comp_ind);

        void
        on_added_component(EntityHandle entity, ComponentIndex comp_ind);

        void
        on_removed_component(EntityHandle entity, ComponentIndex comp_ind);

        std::unique_ptr<RegistryPimpl> pimpl;
    };
}
