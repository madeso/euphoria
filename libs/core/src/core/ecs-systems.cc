#include "core/ecs-systems.h"


namespace euphoria::core::ecs
{
    ComponentSystem::ComponentSystem(const std::string& the_name)
        : name(the_name)
    {}

    void
    ComponentSystemStore::add(std::shared_ptr<ComponentSystem> system)
    {
        systems.emplace_back(system);
    }

    void
    ComponentSystemUpdaterStore::update(Registry* reg, float dt) const
    {
        for(const auto* s: systems)
        {
            s->update(reg, dt);
        }
    }

    void
    ComponentSystemInitializerStore::on_add(entity_id ent) const
    {
        for(const auto* s: systems)
        {
            s->on_add(ent);
        }
    }

    void
    ComponentSystemSpriteDrawerStore::draw(
            Registry* reg,
            render::sprite_renderer* renderer) const
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

    World::World(ecs::Systems* sys) : systems(sys)
    {
        reg.add_callback(make_registry_entity_callback(
                [this](entity_id id) { this->systems->initializer.on_add(id); }));
    }

    void
    World::update(float dt)
    {
        systems->updater.update(&reg, dt);
    }

    void
    World::draw(render::sprite_renderer* renderer)
    {
        systems->sprite_drawer.draw(&reg, renderer);
    }

}
