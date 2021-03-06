#include "core/ecs.h"

#include <algorithm>

#include "core/stdutils.h"
#include "core/log.h"
#include "core/stringmerger.h"
#include "core/functional.h"
#include "core/editdistance.search.h"


namespace euphoria::core::ecs
{
    ////////////////////////////////////////////////////////////////////////////////

    static constexpr auto version_shift_amount = 24;
    static constexpr entity_id entity_bit_mask = 0x00FFFFFF;
    static constexpr entity_id version_bit_mask = 0xFF000000;

    ////////////////////////////////////////////////////////////////////////////////

#if BUILD_ENTITY_DEBUG_COMPONENT == 1
    Component::Component(TypeName n, TypeId i) : type_name(n), type_id(i) {}
#endif

    ////////////////////////////////////////////////////////////////////////////////

    entity_id
    get_value(entity_id id)
    {
        return id & entity_bit_mask;
    }

    entity_version
    get_version(entity_version id)
    {
        return static_cast<entity_version>(id & version_bit_mask) >> version_shift_amount;
    }

    entity_id
    get_id(entity_id id, entity_version version)
    {
        const entity_id masked_id = id & entity_bit_mask;
        const entity_id masked_version = (version << version_shift_amount) & version_bit_mask;
        return masked_id | masked_version;
    }

    ////////////////////////////////////////////////////////////////////////////////

    struct component_list
    {
        std::string name;
        std::map<entity_id, std::shared_ptr<component>> components;

        explicit component_list(const std::string& n) : name(n) {}

        std::shared_ptr<component>
        get_component(entity_id entity)
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
        add_component(entity_id entity, std::shared_ptr<component> data)
        {
            components[entity] = data;
        }

        void
        remove_component(entity_id entity)
        {
            components.erase(entity);
        }

        [[nodiscard]] std::vector<entity_id>
        get_components() const
        {
            const auto keys = get_keys(components);
            ASSERT(get_sorted(keys) == keys);
            return keys;
        }
    };

    ////////////////////////////////////////////////////////////////////////////////

    struct registry_impl
    {
        entity_id current = 0;
        std::vector<entity_id> free_entities;
        std::vector<entity_id> alive;

        entity_id
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
                entity_id last = *free_entities.rbegin();
                free_entities.pop_back();
                const auto n = get_id(get_value(last), get_version(last) + 1);
                set_alive(n);
                return n;
            }
        }

        std::vector<std::shared_ptr<registry_entity_callback>> callbacks;

        void
        post_create(entity_id id)
        {
            for(auto& c: callbacks)
            {
                c->on_created(id);
            }
        }

        void
        add_callback(std::shared_ptr<registry_entity_callback> callback)
        {
            callbacks.emplace_back(callback);
        }

        void
        set_alive(entity_id id)
        {
            LOG_DEBUG("Alive {0}/{1}", get_value(id), get_version(id));
            alive.emplace_back(id);
        }

        [[nodiscard]] bool
        is_alive(entity_id id) const
        {
            return std::find(alive.begin(), alive.end(), id) != alive.end();
        }

        std::vector<entity_id> destroyed_entities;

        void
        destroy_entity(entity_id id)
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

        component_id next_component_id = 0;
        std::map<component_id, std::shared_ptr<component_list>> components;
        std::map<std::string, component_id> name_to_component;

        component_id
        register_new_component_type(const std::string& name)
        {
            const auto ret = next_component_id;
            next_component_id += 1;
            components[ret] = std::make_shared<component_list>(name);
            name_to_component[name] = ret;

            return ret;
        }

        [[nodiscard]] std::string
        get_component_name(component_id id) const
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

        std::shared_ptr<component>
        get_component(entity_id entity, component_id id)
        {
            return components[id]->get_component(entity);
        }

        void
        add_component_to_entity(
                entity_id entity,
                component_id id,
                std::shared_ptr<component> data)
        {
            components[id]->add_component(entity, data);
        }


        std::vector<entity_id>
        get_entities_with_components(const std::vector<component_id>& component_list)
        {
            ASSERT(!component_list.empty());
            bool first = true;
            std::vector<entity_id> r;
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
                    std::vector<entity_id> rr;
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

        result<component_id>
        get_custom_component_by_name(const std::string& name)
        {
            if(const auto found_component = name_to_component.find(name); found_component == name_to_component.end())
            {
                auto matches = string_mergers::english_or.merge
                (
                    map<std::string>
                    (
                        search::find_closest<search::match>
                        (
                            3, name_to_component,
                            [&](const auto& entry) -> search::match
                            {
                                return {entry.first, name};
                            }
                        ),
                        [](const search::match& m)
                        {
                            return m.name;
                        }
                    )
                );
                return result<component_id>::create_error(fmt::format("could be {}", matches));
            }
            else
            {
                return result<component_id>::create_value(found_component->second);
            }
        }
    };

    ////////////////////////////////////////////////////////////////////////////////

    registry::registry() : impl(new registry_impl {}) {}

    registry::~registry() = default;

    entity_id
    registry::create_new_entity()
    {
        return impl->create_new_entity();
    }

    void
    registry::post_create(entity_id id)
    {
        impl->post_create(id);
    }

    void
    registry::add_callback(std::shared_ptr<registry_entity_callback> callback)
    {
        impl->add_callback(callback);
    }

    bool
    registry::is_alive(entity_id id) const
    {
        return impl->is_alive(id);
    }

    void
    registry::destroy_entity(entity_id id)
    {
        impl->destroy_entity(id);
    }


    component_id
    registry::register_new_component_type(const std::string& name)
    {
        return impl->register_new_component_type(name);
    }

    std::string
    registry::get_component_name(component_id id) const
    {
        return impl->get_component_name(id);
    }

    result<component_id>
    registry::get_custom_component_by_name(const std::string& name)
    {
        return impl->get_custom_component_by_name(name);
    }

    std::shared_ptr<component>
    registry::get_component(entity_id entity, component_id component)
    {
        return impl->get_component(entity, component);
    }

    void
    registry::add_component_to_entity(
            entity_id entity,
            component_id component,
            std::shared_ptr<ecs::component> data)
    {
        impl->add_component_to_entity(entity, component, data);
    }

    std::vector<entity_id>
    registry::get_entities_with_components(const std::vector<component_id>& components)
    {
        return impl->get_entities_with_components(components);
    }

    void
    registry::remove_entities_tagged_for_removal()
    {
        impl->remove_entities_tagged_for_removal();
    }


}
