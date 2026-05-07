#include "eu/runner/entity.h"

#include <algorithm>

namespace eu::runner
{


    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Implementations

    void Alive::kill()
    {
        if(health == 0)
        {
            health = 1;
        }
    }

    void Alive::update_for_frame()
    {
        if(health > 0)
        {
            health += 1;
        }
    }

    bool Alive::is_pending_removal() const
    {
        return health > 0;
    }

    bool Alive::delete_owner() const
    {
        return health > 10;
    }


    // ------------------------------------------------------------------------
    // EntitySystemWithPrio

    EntitySystemWithPrio::EntitySystemWithPrio(EntitySystem* s, int p)
        : system(s)
        , prio(p)
    {
    }


    // ------------------------------------------------------------------------
    // EntitySystemUpdateStageList

    void EntitySystemUpdateStageList::update(UpdateStage stage)
    {
        for(auto& es: systems)
        {
            es.system->update(stage);
        }
    }

    void EntitySystemUpdateStageList::add(EntitySystem* sys, int prio)
    {
        systems.emplace_back(sys, prio);
        std::sort(systems.begin(), systems.end(), [](const auto& lhs, const auto& rhs)
        {
            return lhs.prio < rhs.prio;
        });
    }

    void EntitySystemUpdateStageList::remove(EntitySystem* sys)
    {
        core::update_and_erase(&systems,
            [sys](const EntitySystemWithPrio& es)
            { return es.system == sys;}
        );
    }


    // ------------------------------------------------------------------------
    // EntitySystemUpdate

    void EntitySystemUpdate::update(UpdateStage stage)
    {
        systems[static_cast<std::size_t>(stage)].update(stage);
    }

    void EntitySystemUpdate::add(EntitySystem* sys, UpdateStage stage, int prio)
    {
        systems[static_cast<std::size_t>(stage)].add(sys, prio);
    }

    void EntitySystemUpdate::remove(EntitySystem* sys, UpdateStage stage)
    {
        systems[static_cast<std::size_t>(stage)].remove(sys);
    }


    // ------------------------------------------------------------------------
    // ComponentType

    // ------------------------------------------------------------------------
    // ComponentFactory
    void ComponentFactory::add(const ComponentType* ty)
    {
        types.insert({ty->name, ty});
    }
    
    const ComponentType* ComponentFactory::from_name_or_null(core::HashedStringView name) const
    {
        auto found = types.find(name);
        if(found != types.end()) { return found->second; }
        else { return nullptr; }
    }

    // ------------------------------------------------------------------------
    // EntitySystemType

    // ------------------------------------------------------------------------
    // EntitySystemFactory
    void EntitySystemFactory::add(const EntitySystemType* ty)
    {
        types.insert({ty->name, ty});
    }

    const EntitySystemType* EntitySystemFactory::from_name_or_null(core::HashedStringView name)
    {
        auto found = types.find(name);
        if(found != types.end()) { return found->second; }
        else { return nullptr; }
    }

    // ------------------------------------------------------------------------
    // WorldSystemType

    // ------------------------------------------------------------------------
    // WorldSystemFactory
    void WorldSystemFactory::add(const WorldSystemType* ty)
    {
        types.insert({ty->name, ty});
    }

    const WorldSystemType* WorldSystemFactory::from_name_or_null(core::HashedStringView name)
    {
        auto found = types.find(name);
        if(found != types.end()) { return found->second; }
        else { return nullptr; }
    }


    // ------------------------------------------------------------------------
    // Entity

    void Entity::update(UpdateStage stage)
    {
        systems.update(stage);

        if(stage == UpdateStage::end_frame)
        {
            alive.update_for_frame();
            update_and_remove_alives(&components, &dead_components);
        }
    }

    // ------------------------------------------------------------------------
    // Component

    // ------------------------------------------------------------------------
    // SpatialComponent

    void attach(World* world, Entity* parent, Entity* child)
    {
        assert(parent->is_spatial_entity() && child->is_spatial_entity());
        // todo(Gustav): implement attachments
    }

    // ------------------------------------------------------------------------
    // RequestedComponents

    // ------------------------------------------------------------------------
    // EntitySystem

    // ------------------------------------------------------------------------
    // WorldSystem

    // ------------------------------------------------------------------------
    // WorldSystemWithPrio
    WorldSystemWithPrio::WorldSystemWithPrio(WorldSystem* s, int p)
        : system(s)
        , prio(p)
    {
    }

    // ------------------------------------------------------------------------
    // WorldSystemUpdateStageList
    void WorldSystemUpdateStageList::update(UpdateStage stage)
    {
        for(auto& es: systems)
        {
            es.system->update(stage);
        }
    }

    void WorldSystemUpdateStageList::add(WorldSystem* sys, int prio)
    {
        systems.emplace_back(sys, prio);
        std::sort(systems.begin(), systems.end(), [](const auto& lhs, const auto& rhs)
        {
            return lhs.prio < rhs.prio;
        });
    }

    void WorldSystemUpdateStageList::remove(WorldSystem* sys)
    {
        std::erase_if(systems, [sys](const WorldSystemWithPrio& es) { return es.system == sys;});
    }

    // ------------------------------------------------------------------------
    // WorldSystemUpdate

    void WorldSystemUpdate::update(UpdateStage stage)
    {
        systems[static_cast<std::size_t>(stage)].update(stage);
    }

    void WorldSystemUpdate::add(WorldSystem* system, UpdateStage stage, int prio)
    {
        systems[static_cast<std::size_t>(stage)].add(system, prio);
    }

    void WorldSystemUpdate::remove(WorldSystem* system, UpdateStage stage)
    {
        systems[static_cast<std::size_t>(stage)].remove(system);
    }

    // ------------------------------------------------------------------------
    // World

    void World::update(UpdateStage stage)
    {
        // todo(Gustav): implement threading for entity
        // todo(Gustav): take care of updating parent before child
        // parallelized, spatial parent is updated before child (worker threads: nuber of cores - 1)
        // place attached entities on the same thread as parent, schedule parent to update before the child
        for(auto& ent: entities)
        {
            ent->update(stage);
        }
        
        // todo(Gustav): implement threading for world
        // sequential, can use worker threads if needed
        system_update.update(stage);
    }

}
