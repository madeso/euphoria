#include "runner/systems.h"


#include "render/texture.h"
#include "render/spriterender.h"
#include "render/texturecache.h"

#include "runner/ecs.systems.h"
#include "runner/components.h"

namespace euphoria::runner
{
    struct SystemSpriteDraw
        : ComponentSystem
        , ComponentSystemSpriteDrawer
    {
        runner::Components* components;

        explicit SystemSpriteDraw(runner::Components* c)
            : ComponentSystem("sprite draw")
            , components(c)
        {
        }

        void
        draw
        (
            core::ecs::Registry* reg,
            render::SpriteRenderer* renderer
        ) const override
        {
            const auto items = reg->view
            (
                {
                    components->position2,
                    components->sprite
                }
            );
            for(auto ent: items)
            {
                auto& sprite = reg->get_component<ComponentSprite>(ent, components->sprite);
                auto& pos = reg->get_component<ComponentPosition2>(ent, components->position2);
                renderer->draw_sprite
                (
                    *sprite.texture,
                    get_sprite_rect(pos.pos, *sprite.texture)
                );
            }
        }

        void
        register_callbacks(Systems* systems) override
        {
            systems->sprite_drawer.add(this);
        }
    };


    void
    add_systems(Systems* sys, Components* comps)
    {
        sys->add_and_register(std::make_shared<SystemSpriteDraw>(comps));
    }
}
