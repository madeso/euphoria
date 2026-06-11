#include "eu/runner/entity.h"

#include <algorithm>

namespace eu::runner
{

    // ------------------------------------------------------------------------
	//  UpdateStageAndPrio

    // ------------------------------------------------------------------------
	//  SystemWithPrio

    // ------------------------------------------------------------------------
	//  UpdateHandler
    
    // ------------------------------------------------------------------------
	//  Entity

    void Entity::add_component(std::unique_ptr<Component> c)
    {
        components.emplace_back(std::move(c));
        Component* nc = components.back().get();
        for(auto& sys: systems)
        {
            sys->add_component(nc);
        }
    }
    void Entity::add_system(std::unique_ptr<EntitySystem> system)
    {
        systems.emplace_back(std::move(system));
        EntitySystem* ne = systems.back().get();
        updates.add(ne);
        for (auto& c: components)
        {
            ne->add_component(c.get());
        }
    }

    void Entity::add_tag(const Hsh& h)
    {
        tags.add(h);
    }

    bool Entity::has_tag(const Hsh& h) const
    {
        return tags.contains(h);
    }

    // ------------------------------------------------------------------------
	//  Component
    const HshSt& Component::type()
    {
        static const auto r = HshSt{ .data = {}, .primary = "<Component>"sv};
        return r;
    }

    const HshSt& Component::get_type()
    {
        return type();
    }

    // ------------------------------------------------------------------------
	//  SpatialComponent

    EU_IMP_COMPONENT_TYPE(SpatialComponent, Component)

    void SpatialComponent::set_transform(const m4& t)
    {
        transform = t;
    }
    const m4& SpatialComponent::get_transform() const
    {
        return transform;
    }

    // ------------------------------------------------------------------------
	//  EntitySystem


    // ------------------------------------------------------------------------
    // EntitySystemUpdateStageList

    // ---------------------------------------------------
    // World

    Entity* World::add_entity()
    {
        entities.emplace_back(std::make_unique<Entity>());
        return entities.back().get();
    }

    void World::on_add_component(Entity* entity, Component* component)
    {
        for (auto& sys: systems)
        {
            sys->add_component(entity, component);
        }
    }

    void World::add_system(std::unique_ptr<WorldSystem> system)
    {
        systems.emplace_back(std::move(system));
        WorldSystem* sys = systems.back().get();

        updates.add(sys);
        for (auto& ent: entities)
        {
            for (auto& comp: ent->components) {
                sys->add_component(ent.get(), comp.get());
            }
        }
    }

    void World::update(UpdateStage stage, float dt)
    {
        for (auto& ent: entities)
        {
            ent->updates.update(stage, dt);
        }
    }
}
