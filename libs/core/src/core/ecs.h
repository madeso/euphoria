#pragma once


#include <optional>
#include <memory>
#include <string_view>
#include <utility>  // std::forward
#include <typeinfo>

#include "assert/assert.h"

#include "core/ecs.id.h"

namespace eu::core::ecs
{
    // todo(Gustav): move detail to detail namespace
    std::size_t c_ent(EntityHandle v);
    std::size_t c_comp(ComponentIndex v);

    template<typename T>
    constexpr std::string_view get_component_base_name()
    {
        return typeid(T).name();
    }

    struct ComponentArrayBase
    {
        std::string_view name;

        ComponentArrayBase(std::string_view n);
        virtual ~ComponentArrayBase() = default;

        ComponentArrayBase(const ComponentArrayBase&) = delete;
        ComponentArrayBase(ComponentArrayBase&&) = delete;
        ComponentArrayBase& operator=(const ComponentArrayBase&) = delete;
        ComponentArrayBase& operator=(ComponentArrayBase&&) = delete;

        
        virtual void remove(EntityHandle) = 0;
        [[nodiscard]] virtual bool has(EntityHandle) const= 0;
    };

    // contains a list of components for a single component type
    template<typename T>
    struct GenericComponentArray : public ComponentArrayBase
    {
        // todo(Gustav): implement better sparse vector
        std::vector<std::optional<T>> components;

        GenericComponentArray()
            : ComponentArrayBase(get_component_base_name<T>())
        {
        }

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

        T&
        get(EntityHandle entity)
        {
            const auto index = c_ent(entity);
            ASSERT(components.size() >= index);
            ASSERT(components[index].has_value());
            return *components[index];
        }

        T*
        get_or_null(EntityHandle entity)
        {
            const auto index = c_ent(entity);
            if(has_value_for_index(index))
            {
                return &(*components[index]);
            }
            else
            {
                return nullptr;
            }
        }

        // todo(Gustav): rename to has_value_for_index
        [[nodiscard]]
        bool
        has_value_for_index(std::size_t index) const
        {
            return false ==
            (
                components.size() <= index ||
                components[index].has_value() == false
            );
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

        [[nodiscard]]
        bool
        has(EntityHandle entity) const override
        {
            const auto index = c_ent(entity);
            return has_value_for_index(index);
        }
    };


    ///////////////////////////////////////////////////////////////////////////


    struct RegistryPimpl;


    struct Registry
    {
        Registry();
        ~Registry();

        Registry(const Registry&) = delete;
        Registry(Registry&&) = delete;
        Registry& operator=(const Registry&) = delete;
        Registry& operator=(Registry&&) = delete;

        EntityHandle create();
        void destroy(EntityHandle entity);
        ComponentIndex set_component_array(const std::string& name, std::unique_ptr<ComponentArrayBase>&& components);

        
        template<typename T>
        ComponentIndex register_component(const std::string& name)
        {
            return set_component_array(name, std::make_unique<GenericComponentArray<T>>());
        }


        template<typename T>
        void add_component(EntityHandle entity, ComponentIndex comp_ind, T&& component)
        {
            get_components<T>(comp_ind)->add(entity, std::forward<T>(component));
            on_added_component(entity, comp_ind);
        }


        template<typename T>
        void remove_component(EntityHandle entity, ComponentIndex comp_ind)
        {
            on_removed_component(entity, comp_ind);
            get_components<T>(comp_ind)->remove(entity);
        }


        template<typename T>
        T& get_component(EntityHandle entity, ComponentIndex comp_ind)
        {
            return get_components<T>(comp_ind)->get(entity);
        }

        template<typename T>
        T* get_component_or_null(EntityHandle entity, ComponentIndex comp_ind)
        {
            return get_components<T>(comp_ind)->get_or_null(entity);
        }

        [[nodiscard]] std::string get_component_debug_name(ComponentIndex c) const;
        [[nodiscard]] bool has_component(EntityHandle entity, ComponentIndex comp_ind) const;
        [[nodiscard]] std::vector<EntityHandle> view(const std::vector<ComponentIndex>& matching_components) const;

        [[nodiscard]] int get_number_of_active_entities() const;
    private:
        std::unique_ptr<RegistryPimpl> pimpl;

        template<typename T>
        [[nodiscard]] GenericComponentArray<T>* get_components(ComponentIndex comp_ind)
        {
            ComponentArrayBase* base = get_components_base(comp_ind);
            ASSERTX(base->name == get_component_base_name<T>(), base->name, get_component_base_name<T>(), get_component_name(comp_ind));
            return static_cast<GenericComponentArray<T>*>(base);
        }

        ComponentArrayBase* get_components_base(ComponentIndex comp_ind);
        void on_added_component(EntityHandle entity, ComponentIndex comp_ind);
        void on_removed_component(EntityHandle entity, ComponentIndex comp_ind);

        [[nodiscard]] std::string get_component_name(ComponentIndex comp_ind) const;
    };
}
