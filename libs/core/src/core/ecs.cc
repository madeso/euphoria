#include "core/ecs.h"

#include <algorithm>

#include "core/stdutils.h"
#include "log/log.h"
#include "core/stringmerger.h"
#include "core/functional.h"
#include "core/editdistance.search.h"


namespace euphoria::core::ecs
{
    ////////////////////////////////////////////////////////////////////////////////

    static constexpr auto version_shift_amount = 6 * 4;
    static constexpr EntityId entity_bit_mask = 0x00FFFFFF;
    static constexpr EntityId version_bit_mask = 0xFF000000;

    ////////////////////////////////////////////////////////////////////////////////

#if BUILD_ENTITY_DEBUG_COMPONENT == 1
    Component::Component(TypeName n, TypeId i) : type_name(n), type_id(i) {}
#endif

    ////////////////////////////////////////////////////////////////////////////////

    EntityId
    get_value(EntityId id)
    {
        return id & entity_bit_mask;
    }

    EntityVersion
    get_version(EntityId id)
    {
        return static_cast<EntityVersion>((id & version_bit_mask) >> version_shift_amount);
    }

    EntityId
    get_id(EntityId id, EntityVersion version)
    {
        const EntityId masked_id = id & entity_bit_mask;
        const EntityId masked_version = (version << version_shift_amount) & version_bit_mask;
        return masked_id | masked_version;
    }

    ////////////////////////////////////////////////////////////////////////////////

    struct ComponentList
    {
        std::string name;
        std::map<EntityId, std::shared_ptr<Component>> components;

        explicit ComponentList(const std::string& n) : name(n) {}

        std::shared_ptr<Component>
        get_component(EntityId entity)
        {
            auto found = components.find(entity);
            if(found != components.end())
            {
                return found->second;
            }
            else
            {
                return nullptr;
            }
        }

        void
        add_component(EntityId entity, std::shared_ptr<Component> data)
        {
            components[entity] = data;
        }

        void
        remove_component(EntityId entity)
        {
            components.erase(entity);
        }

        [[nodiscard]] std::vector<EntityId>
        get_components() const
        {
            const auto keys = get_keys(components);
            ASSERT(get_sorted(keys) == keys);
            return keys;
        }
    };

    ////////////////////////////////////////////////////////////////////////////////

    struct RegistryImplementation
    {
        EntityId current = 0;
        std::vector<EntityId> free_entities;
        std::vector<EntityId> alive;

        EntityId
        create_new_entity()
        {
            if(free_entities.empty())
            {
                current += 1;
                set_alive(current);
                return current;
            }
            else
            {
                EntityId last = *free_entities.rbegin();
                free_entities.pop_back();
                const auto n = get_id(get_value(last), get_version(last) + 1);
                set_alive(n);
                return n;
            }
        }

        std::vector<std::shared_ptr<RegistryEntityCallback>> callbacks;

        void
        post_create(EntityId id)
        {
            for(auto& c: callbacks)
            {
                c->on_created(id);
            }
        }

        void
        add_callback(std::shared_ptr<RegistryEntityCallback> callback)
        {
            callbacks.emplace_back(callback);
        }

        void
        set_alive(EntityId id)
        {
            LOG_DEBUG("Alive {0}/{1}", get_value(id), get_version(id));
            alive.emplace_back(id);
        }

        [[nodiscard]] bool
        is_alive(EntityId id) const
        {
            return std::find(alive.begin(), alive.end(), id) != alive.end();
        }

        std::vector<EntityId> destroyed_entities;

        void
        destroy_entity(EntityId id)
        {
            LOG_DEBUG("Destroy {0}/{1}", get_value(id), get_version(id));
            destroyed_entities.emplace_back(id);
        }

        void
        remove_entities_tagged_for_removal()
        {
            for(const auto id: destroyed_entities)
            {
                // todo(Gustav): postpone!
                if(swap_back_and_erase_object(id, &alive))
                {
                    for(const auto& entry: components)
                    {
                        entry.second->remove_component(id);
                    }
                    free_entities.emplace_back(id);
                }
            }
            destroyed_entities.resize(0);
        }

        ComponentId next_component_id = 0;
        std::map<ComponentId, std::shared_ptr<ComponentList>> components;
        std::map<std::string, ComponentId> name_to_component;

        ComponentId
        register_new_component_type(const std::string& name)
        {
            const auto ret = next_component_id;
            next_component_id += 1;
            components[ret] = std::make_shared<ComponentList>(name);
            name_to_component[name] = ret;

            return ret;
        }

        [[nodiscard]] std::string
        get_component_name(ComponentId id) const
        {
            auto found = components.find(id);
            if(found == components.end())
            {
                return "<unknown>";
            }
            else
            {
                return (found->second)->name;
            }
        }

        std::shared_ptr<Component>
        get_component(EntityId entity, ComponentId id)
        {
            return components[id]->get_component(entity);
        }

        void
        add_component_to_entity
        (
            EntityId entity,
            ComponentId id,
            std::shared_ptr<Component> data
        )
        {
            components[id]->add_component(entity, data);
        }


        std::vector<EntityId>
        get_entities_with_components(const std::vector<ComponentId>& component_list)
        {
            ASSERT(!component_list.empty());
            bool first = true;
            std::vector<EntityId> r;
            for(const auto c: component_list)
            {
                const auto v = components[c]->get_components();
                if(first)
                {
                    r = v;
                    first = false;
                }
                else
                {
                    std::vector<EntityId> rr;
                    std::set_intersection
                    (
                        v.begin(),
                        v.end(),
                        r.begin(),
                        r.end(),
                        std::back_inserter(rr)
                    );
                    r = rr;
                }
            }

            return r;
        }

        Result<ComponentId>
        get_custom_component_by_name(const std::string& name)
        {
            if(const auto found_component = name_to_component.find(name); found_component == name_to_component.end())
            {
                auto matches = string_mergers::english_or.merge
                (
                    map<std::string>
                    (
                        search::find_closest<search::Match>
                        (
                            3, name_to_component,
                            [&](const auto& entry) -> search::Match
                            {
                                return {entry.first, name};
                            }
                        ),
                        [](const search::Match& m)
                        {
                            return m.name;
                        }
                    )
                );
                return Result<ComponentId>::create_error(fmt::format("could be {}", matches));
            }
            else
            {
                return Result<ComponentId>::create_value(found_component->second);
            }
        }
    };

    ////////////////////////////////////////////////////////////////////////////////

    Registry::Registry() : impl(new RegistryImplementation {}) {}

    Registry::~Registry() = default;

    EntityId
    Registry::create_new_entity()
    {
        return impl->create_new_entity();
    }

    void
    Registry::post_create(EntityId id)
    {
        impl->post_create(id);
    }

    void
    Registry::add_callback(std::shared_ptr<RegistryEntityCallback> callback)
    {
        impl->add_callback(callback);
    }

    bool
    Registry::is_alive(EntityId id) const
    {
        return impl->is_alive(id);
    }

    void
    Registry::destroy_entity(EntityId id)
    {
        impl->destroy_entity(id);
    }


    ComponentId
    Registry::register_new_component_type(const std::string& name)
    {
        return impl->register_new_component_type(name);
    }

    std::string
    Registry::get_component_name(ComponentId id) const
    {
        return impl->get_component_name(id);
    }

    Result<ComponentId>
    Registry::get_custom_component_by_name(const std::string& name)
    {
        return impl->get_custom_component_by_name(name);
    }

    std::shared_ptr<Component>
    Registry::get_component(EntityId entity, ComponentId component)
    {
        return impl->get_component(entity, component);
    }

    void
    Registry::add_component_to_entity(
            EntityId entity,
            ComponentId component,
            std::shared_ptr<ecs::Component> data)
    {
        impl->add_component_to_entity(entity, component, data);
    }

    std::vector<EntityId>
    Registry::get_entities_with_components(const std::vector<ComponentId>& components)
    {
        return impl->get_entities_with_components(components);
    }

    void
    Registry::remove_entities_tagged_for_removal()
    {
        impl->remove_entities_tagged_for_removal();
    }


}
