#include "core/ecs2.h"


#include <unordered_map>

#include "core/cint.h"


namespace euphoria::core::ecs
{
    // what components a entity has
    struct Signature
    {
        std::vector<bool> components;

        bool has_component(ComponentIndex c) const
        {
            if(c < components.size()) { return components[c]; }
            else { return false; }
        }

        void set_component(ComponentIndex c, bool has)
        {
            components.resize(c+1, false);
            components[c] = has;
        }

        void destroy()
        {
            components.clear();
        }
    };

    // pool of "alive" entities
    struct AliveEntities
    {
        EntityHandle create()
        {
            if(free_handles.empty())
            {
                const auto r = signatures.size();
                signatures.emplace_back();
                return static_cast<EntityHandle>(r);
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

        std::vector<EntityHandle> view(const std::vector<ComponentIndex>& matching_components) const
        {
            ASSERT(matching_components.empty() == false);

            std::vector<EntityHandle> r;
            for(int i=0; i<c_sizet_to_int(signatures.size()); i += 1)
            {
                if(has_components(static_cast<EntityHandle>(i), matching_components))
                {
                    r.emplace_back(static_cast<EntityHandle>(i));
                }
            }
            return r;
        }

        void set_component(EntityHandle handle, ComponentIndex component, bool has)
        {
            ASSERT(handle < signatures.size());
            signatures[handle].set_component(component, has);
        }

        bool has_components(EntityHandle handle, const std::vector<ComponentIndex>& components) const
        {
            ASSERT(components.empty() == false);

            const auto& sig = signatures[handle];
            for(const auto comp: components)
            {
                if(sig.has_component(comp) == false)
                {
                    return false;
                }
            }
            return true;
        }

        [[nodiscard]] int
        get_count() const
        {
            const auto sign_count = c_sizet_to_int(signatures.size());
            const auto free_count = c_sizet_to_int(free_handles.size());
            ASSERT(sign_count >= free_count);
            return sign_count - free_count;
        }

        std::vector<Signature> signatures;
        std::vector<EntityHandle> free_handles;
    };

    template<typename T>
    std::string get_component_name()
    {
        return typeid(T).name();
    }

    // map (internal) name -> index && index -> (debug) name
    struct KnownComponentTypes
    {
        std::unordered_map<std::string, ComponentIndex> name_to_index;
        std::vector<std::string> index_to_name;

        ComponentIndex add(const std::string& name)
        {
            const auto index = index_to_name.size();
            
            name_to_index.insert({name, index});
            index_to_name.emplace_back(name);
            
            return index;
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

        std::vector<std::unique_ptr<ComponentArrayBase>> component_arrays;

        ComponentArrayBase* get_components_base(ComponentIndex comp_ind)
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

        std::vector<EntityHandle> view(const std::vector<ComponentIndex>& matching_components) const
        {
            return alive_entities.view(matching_components);
        }

        ComponentArrayBase* get_components_base(ComponentIndex comp_ind)
        {
            return all_components.get_components_base(comp_ind);
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

    void Registry::destroy(EntityHandle entity)
    {
        pimpl->destroy(entity);
    }

    ComponentIndex Registry::set_component_array(const std::string& name, std::unique_ptr<ComponentArrayBase>&& components)
    {
        return pimpl->set_component_array(name, std::move(components));
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
