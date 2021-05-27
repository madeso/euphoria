#include "core/ecs-systems.h"


namespace euphoria::core::ecs
{
    component_system::component_system(const std::string& the_name)
        : name(the_name)
    {}

    void
    component_system_store::add(std::shared_ptr<component_system> system)
    {
        systems.emplace_back(system);
    }

    void
    component_system_updater_store::update(registry* reg, float dt) const
    {
        for(const auto* s: systems)
        {
            s->update(reg, dt);
        }
    }

    void
    component_system_initializer_store::on_add(entity_id ent) const
    {
        for(const auto* s: systems)
        {
            s->on_add(ent);
        }
    }

    void
    component_system_sprite_drawer_store::draw(
            registry* reg,
            render::sprite_renderer* renderer) const
    {
        for(const auto* s: systems)
        {
            s->draw(reg, renderer);
        }
    }

    void
    systems::add_and_register(std::shared_ptr<component_system> system)
    {
        store.add(system);
        system->register_callbacks(this);
    }

    world::world(ecs::systems* sys) : systems(sys)
    {
        reg.add_callback(make_registry_entity_callback(
                [this](entity_id id) { this->systems->initializer.on_add(id); }));
    }

    void
    world::update(float dt)
    {
        systems->updater.update(&reg, dt);
    }

    void
    world::draw(render::sprite_renderer* renderer)
    {
        systems->sprite_drawer.draw(&reg, renderer);
    }

} // namespace euphoria::core::ecs
