#include "engine/ecs.systems.h"


namespace euphoria::engine
{
    ComponentSystem::ComponentSystem(const std::string& the_name)
        : name(the_name)
    {
    }

    void
    ComponentSystemStore::add(std::shared_ptr<ComponentSystem> system)
    {
        systems.emplace_back(system);
    }

    void
    ComponentSystemUpdaterStore::update(core::ecs::Registry* reg, float dt) const
    {
        for(const auto* s: systems)
        {
            s->update(reg, dt);
        }
    }

    void
    ComponentSystemInitializerStore::on_add(core::ecs::EntityHandle ent) const
    {
        for(const auto* s: systems)
        {
            s->on_add(ent);
        }
    }

    void
    ComponentSystemSpriteDrawerStore::draw
    (
        core::ecs::Registry* reg,
        render::SpriteRenderer* renderer
    ) const
    {
        for(const auto* s: systems)
        {
            s->draw(reg, renderer);
        }
    }

    void
    Systems::add_and_register(std::shared_ptr<ComponentSystem> system)
    {
        store.add(system);
        system->register_callbacks(this);
    }

    World::World(Systems* sys)
        : systems(sys)
    {
    }

    void
    World::update(float dt)
    {
        systems->updater.update(&reg, dt);
    }

    void
    World::draw(render::SpriteRenderer* renderer)
    {
        systems->sprite_drawer.draw(&reg, renderer);
    }

    void World::post_create(core::ecs::EntityHandle id) const
    {
        systems->initializer.on_add(id);
    }

}
