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
    struct systems;

    // only a base class container
    struct component_system
    {
        explicit component_system(const std::string& the_name);
        virtual ~component_system() = default;

        component_system(const component_system&) = delete;
        component_system(component_system&&) = delete;
        void operator=(const component_system&) = delete;
        void operator=(component_system&&) = delete;

        virtual void
        register_callbacks(systems* systems) = 0;

        // for debug purposes
        const std::string name;
    };

    struct component_system_store
    {
        component_system_store() = default;

        void
        add(std::shared_ptr<component_system> system);

    private:
        std::vector<std::shared_ptr<component_system>> systems;
    };

    struct component_system_updater
    {
        component_system_updater() = default;
        virtual ~component_system_updater() = default;

        component_system_updater(const component_system_updater&) = delete;
        component_system_updater(component_system_updater&&) = delete;
        void operator=(const component_system_updater&) = delete;
        void operator=(component_system_updater&&) = delete;

        virtual void
        update(registry* reg, float dt) const = 0;
    };

    struct component_system_initializer
    {
        component_system_initializer() = default;
        virtual ~component_system_initializer() = default;

        component_system_initializer(const component_system_initializer&) = delete;
        component_system_initializer(component_system_initializer&&) = delete;
        void operator=(const component_system_initializer&) = delete;
        void operator=(component_system_initializer&&) = delete;

        virtual void
        on_add(entity_id entity) const = 0;
    };

    struct component_system_sprite_drawer
    {
        component_system_sprite_drawer() = default;
        virtual ~component_system_sprite_drawer() = default;

        component_system_sprite_drawer(const component_system_sprite_drawer&) = delete;
        component_system_sprite_drawer(component_system_sprite_drawer&&) = delete;
        void operator=(const component_system_sprite_drawer&) = delete;
        void operator=(component_system_sprite_drawer&&) = delete;

        virtual void
        draw(registry* reg, render::SpriteRenderer* renderer) const = 0;
    };

    template <typename TSystem>
    struct system_store
    {
        void
        add(TSystem* system)
        {
            systems.emplace_back(system);
        }

        std::vector<TSystem*> systems;
    };

    struct component_system_updater_store : public system_store<component_system_updater>
    {
        void
        update(registry* reg, float dt) const;
    };

    struct component_system_initializer_store : public system_store<component_system_initializer>
    {
        void
        on_add(entity_id ent) const;
    };

    struct component_system_sprite_drawer_store : public system_store<component_system_sprite_drawer>
    {
        void
        draw(registry* reg, render::SpriteRenderer* renderer) const;
    };

    struct systems
    {
        void
        add_and_register(std::shared_ptr<component_system> system);

        // stores the system
        component_system_store store;

        // system references for various global callbacks
        component_system_updater_store updater;
        component_system_initializer_store  initializer;
        component_system_sprite_drawer_store sprite_drawer;
    };

    struct world
    {
        registry   reg;
        ecs::systems* systems;

        explicit world(ecs::systems* sys);

        void
        update(float dt);

        void
        draw(render::SpriteRenderer* renderer);
    };

}
