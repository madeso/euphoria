#include "core/ecs.h"


#include <unordered_map>
#include <algorithm>

#include "core/cint.h"


namespace eu::core::ecs
{
    std::size_t c_comp(ComponentIndex v) { return static_cast<std::size_t>(v); }
    std::size_t c_ent(EntityHandle v) { return static_cast<std::size_t>(v); }
    EntityHandle c_ent(std::size_t v) { return static_cast<EntityHandle>(v); }
    ComponentIndex c_comp(std::size_t v) { return static_cast<ComponentIndex>(v); }


    ComponentArrayBase::ComponentArrayBase(std::string_view n)
        : name(n)
    {
    }


    // what components a entity has
    struct Signature
    {
        std::vector<bool> components;

        void set_component(ComponentIndex c, bool has)
        {
            components.resize(c+1, false);
            components[c] = has;
        }

        void destroy()
        {
            components.clear();
        }

        [[nodiscard]] bool has_component(ComponentIndex c) const
        {
            if (c < components.size()) { return components[c]; }
            else { return false; }
        }
    };

    // pool of "alive" entities
    struct AliveEntities
    {
        std::vector<Signature> signatures;
        std::vector<EntityHandle> free_handles;

        EntityHandle create()
        {
            if(free_handles.empty())
            {
                const auto r = signatures.size();
                signatures.emplace_back();
                return c_ent(r);
            }
            else
            {
                const auto r = free_handles.back();
                free_handles.pop_back();
                return r;
            }
        }

        void destroy(EntityHandle h)
        {
            signatures[h].destroy();
            free_handles.emplace_back(h);
        }

        void set_component(EntityHandle handle, ComponentIndex component, bool has)
        {
            ASSERT(handle < signatures.size());
            signatures[handle].set_component(component, has);
        }

        [[nodiscard]] std::vector<EntityHandle> view(const std::vector<ComponentIndex>& matching_components) const
        {
            ASSERT(matching_components.empty() == false);

            std::vector<EntityHandle> r;
            for(int signature_index=0; signature_index<c_sizet_to_int(signatures.size()); signature_index += 1)
            {
                if(has_components(c_ent(signature_index), matching_components))
                {
                    r.emplace_back(c_ent(signature_index));
                }
            }
            return r;
        }

        [[nodiscard]] bool has_components(EntityHandle handle, const std::vector<ComponentIndex>& components) const
        {
            ASSERT(components.empty() == false);

            const auto& sig = signatures[handle];
            return std::all_of
            (
                components.begin(), components.end(),
                [&](ComponentIndex comp) -> bool
                {
                    return sig.has_component(comp);
                }
            );
        }

        [[nodiscard]] int
        get_count() const
        {
            const auto sign_count = c_sizet_to_int(signatures.size());
            const auto free_count = c_sizet_to_int(free_handles.size());
            ASSERT(sign_count >= free_count);
            return sign_count - free_count;
        }
    };

    // map (internal) name -> index && index -> (debug) name
    struct KnownComponentTypes
    {
        std::unordered_map<std::string, ComponentIndex> name_to_index;
        std::vector<std::string> index_to_name;

        ComponentIndex add(const std::string& name)
        {
            const auto component = c_comp(index_to_name.size());
            
            name_to_index.insert({name, component});
            index_to_name.emplace_back(name);
            
            return component;
        }

        ComponentIndex get_index(const std::string& name)
        {
            const auto found = name_to_index.find(name);
            ASSERT(found != name_to_index.end());
            return found->second;
        }

        std::string get_name(ComponentIndex c)
        {
            return index_to_name[c];
        }
    };

    // contains all the components for all entities
    struct AllComponents
    {
        std::vector<std::unique_ptr<ComponentArrayBase>> component_arrays;

        void set_component_array(ComponentIndex comp_ind, std::unique_ptr<ComponentArrayBase>&& components)
        {
            if(component_arrays.size() <= comp_ind)
            {
                component_arrays.resize(comp_ind + 1);
            }
            component_arrays[comp_ind] = std::move(components);
        }

        void destroy_entity(EntityHandle entity)
        {
            for (auto const& comp : component_arrays)
            {
                comp->remove(entity);
            }
        }

        ComponentArrayBase* get_components_base(ComponentIndex comp_ind)
        {
            ASSERT(comp_ind < component_arrays.size() && "Component not registered before use.");
            ComponentArrayBase* r = component_arrays[comp_ind].get();
            ASSERT(r);
            return r;
        }

        [[nodiscard]] const ComponentArrayBase* get_components_base(ComponentIndex comp_ind) const
        {
            ASSERT(comp_ind < component_arrays.size() && "Component not registered before use.");
            ComponentArrayBase* r = component_arrays[comp_ind].get();
            ASSERT(r);
            return r;
        }
    };

    struct RegistryPimpl
    {
        AliveEntities alive_entities;
        KnownComponentTypes known_component_types;
        AllComponents all_components;

        EntityHandle create()
        {
            return alive_entities.create();
        }

        void destroy(EntityHandle entity)
        {
            all_components.destroy_entity(entity);
            alive_entities.destroy(entity);
        }

        ComponentIndex set_component_array(const std::string& name, std::unique_ptr<ComponentArrayBase>&& components)
        {
            const auto comp_ind = known_component_types.add(name);
            all_components.set_component_array(comp_ind, std::move(components));
            return comp_ind;
        }

        ComponentArrayBase* get_components_base(ComponentIndex comp_ind)
        {
            return all_components.get_components_base(comp_ind);
        }

        std::vector<EntityHandle> view(const std::vector<ComponentIndex>& matching_components) const
        {
            return alive_entities.view(matching_components);
        }
    };


    Registry::Registry()
        : pimpl(std::make_unique<RegistryPimpl>())
    {
    }

    Registry::~Registry()
    {
        pimpl.reset();
    }

    int
    Registry::get_number_of_active_entities() const
    {
        return pimpl->alive_entities.get_count();
    }

    EntityHandle Registry::create()
    {
        return pimpl->create();
    }

    [[nodiscard]] std::string
    Registry::get_component_debug_name(ComponentIndex c) const
    {
        return pimpl->known_component_types.get_name(c);
    }

    void Registry::destroy(EntityHandle entity)
    {
        pimpl->destroy(entity);
    }

    [[nodiscard]] bool
    Registry::has_component(EntityHandle entity, ComponentIndex comp_ind) const
    {
        return pimpl->all_components.get_components_base(comp_ind)->has(entity);
    }

    ComponentIndex Registry::set_component_array(const std::string& name, std::unique_ptr<ComponentArrayBase>&& components)
    {
        return pimpl->set_component_array(name, std::move(components));
    }

    std::string
    Registry::get_component_name(ComponentIndex comp_ind) const
    {
        return pimpl->known_component_types.get_name(comp_ind);
    }

    ComponentArrayBase* Registry::get_components_base(ComponentIndex comp_ind)
    {
        return pimpl->get_components_base(comp_ind);
    }

    std::vector<EntityHandle> Registry::view(const std::vector<ComponentIndex>& matching_components) const
    {
        return pimpl->view(matching_components);
    }

    void
    Registry::on_added_component(EntityHandle entity, ComponentIndex comp_ind)
    {
        pimpl->alive_entities.set_component(entity, comp_ind, true);
    }

    void
    Registry::on_removed_component(EntityHandle entity, ComponentIndex comp_ind)
    {
        pimpl->alive_entities.set_component(entity, comp_ind, false);
    }
}
