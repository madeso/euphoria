#pragma once

#include <memory>

#include <string>
#include <vector>

#include "core/ecs.h"
#include "core/ecs-id.h"

namespace euphoria::render
{
    // its horrible to reference in another module,
    // but for now we only need a pointer to pass around
    // todo(Gustav): fix this by merging in more of the renderer into core?
    struct SpriteRenderer;
}


namespace euphoria::core::ecs
{
    struct Systems;

    // only a base class container
    struct ComponentSystem
    {
        explicit ComponentSystem(const std::string& the_name);
        virtual ~ComponentSystem() = default;

        ComponentSystem(const ComponentSystem&) = delete;
        ComponentSystem(ComponentSystem&&) = delete;
        void operator=(const ComponentSystem&) = delete;
        void operator=(ComponentSystem&&) = delete;

        virtual void
        register_callbacks(Systems* systems) = 0;

        // for debug purposes
        const std::string name;
    };

    struct ComponentSystemStore
    {
        ComponentSystemStore() = default;

        void
        add(std::shared_ptr<ComponentSystem> system);

    private:
        std::vector<std::shared_ptr<ComponentSystem>> systems;
    };

    struct ComponentSystemUpdater
    {
        ComponentSystemUpdater() = default;
        virtual ~ComponentSystemUpdater() = default;

        ComponentSystemUpdater(const ComponentSystemUpdater&) = delete;
        ComponentSystemUpdater(ComponentSystemUpdater&&) = delete;
        void operator=(const ComponentSystemUpdater&) = delete;
        void operator=(ComponentSystemUpdater&&) = delete;

        virtual void
        update(Registry* reg, float dt) const = 0;
    };

    struct ComponentSystemInitializer
    {
        ComponentSystemInitializer() = default;
        virtual ~ComponentSystemInitializer() = default;

        ComponentSystemInitializer(const ComponentSystemInitializer&) = delete;
        ComponentSystemInitializer(ComponentSystemInitializer&&) = delete;
        void operator=(const ComponentSystemInitializer&) = delete;
        void operator=(ComponentSystemInitializer&&) = delete;

        virtual void
        on_add(entity_id entity) const = 0;
    };

    struct ComponentSystemSpriteDrawer
    {
        ComponentSystemSpriteDrawer() = default;
        virtual ~ComponentSystemSpriteDrawer() = default;

        ComponentSystemSpriteDrawer(const ComponentSystemSpriteDrawer&) = delete;
        ComponentSystemSpriteDrawer(ComponentSystemSpriteDrawer&&) = delete;
        void operator=(const ComponentSystemSpriteDrawer&) = delete;
        void operator=(ComponentSystemSpriteDrawer&&) = delete;

        virtual void
        draw(Registry* reg, render::SpriteRenderer* renderer) const = 0;
    };

    template <typename TSystem>
    struct SystemStore
    {
        void
        add(TSystem* system)
        {
            systems.emplace_back(system);
        }

        std::vector<TSystem*> systems;
    };

    struct ComponentSystemUpdaterStore : public SystemStore<ComponentSystemUpdater>
    {
        void
        update(Registry* reg, float dt) const;
    };

    struct ComponentSystemInitializerStore : public SystemStore<ComponentSystemInitializer>
    {
        void
        on_add(entity_id ent) const;
    };

    struct ComponentSystemSpriteDrawerStore : public SystemStore<ComponentSystemSpriteDrawer>
    {
        void
        draw(Registry* reg, render::SpriteRenderer* renderer) const;
    };

    struct Systems
    {
        void
        add_and_register(std::shared_ptr<ComponentSystem> system);

        // stores the system
        ComponentSystemStore store;

        // system references for various global callbacks
        ComponentSystemUpdaterStore updater;
        ComponentSystemInitializerStore initializer;
        ComponentSystemSpriteDrawerStore sprite_drawer;
    };

    struct World\
    {
        Registry reg;
        ecs::Systems* systems;

        explicit World(ecs::Systems* sys);

        void
        update(float dt);

        void
        draw(render::SpriteRenderer* renderer);
    };

}
