#include "engine/systems.h"

#include "core/ecs-systems.h"

#include "render/texture.h"
#include "render/spriterender.h"
#include "render/texturecache.h"

#include "engine/components.h"

namespace euphoria::engine
{
    struct system_sprite_draw
        : core::ecs::component_system
        , core::ecs::component_system_sprite_drawer
    {
        engine::components* components;

        explicit system_sprite_draw(engine::components* c)
            : component_system("sprite draw")
            , components(c)
        {
        }

        void
        draw
        (
            core::ecs::registry* reg,
            render::sprite_renderer* renderer
        ) const override
        {
            const auto items = reg->get_entities_with_components
            (
                std::vector<core::ecs::component_id>
                {
                    components->position2,
                    components->sprite
                }
            );
            for(auto ent: items)
            {
                auto* sprite = reg->get_component_or_null<component_sprite>(ent, components->sprite);
                auto* pos = reg->get_component_or_null<component_position2>(ent, components->position2);
                ASSERT(sprite);
                ASSERT(pos);
                renderer->draw_sprite
                (
                    *sprite->texture,
                    get_sprite_rect(pos->pos, *sprite->texture)
                );
            }
        }

        void
        register_callbacks(core::ecs::systems* systems) override
        {
            systems->sprite_drawer.add(this);
        }
    };


    void
    add_systems(core::ecs::systems* systems, Sol* duk, components* components)
    {
        systems->add_and_register(std::make_shared<system_sprite_draw>(components));
    }
}
