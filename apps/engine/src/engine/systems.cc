#include "engine/systems.h"

#include "core/ecs-systems.h"

#include "render/texture.h"
#include "render/spriterender.h"
#include "render/texturecache.h"

#include "engine/components.h"

namespace euphoria::engine
{
    struct SystemSpriteDraw
        : public core::ecs::component_system
        , public core::ecs::component_system_sprite_drawer
    {
        Components* components;

        explicit SystemSpriteDraw(Components* c)
            : component_system("sprite draw"), components(c)
        {}

        void
        draw(core::ecs::registry*      reg,
             render::sprite_renderer* renderer) const override
        {
            const auto items = reg->get_entities_with_components(std::vector<core::ecs::component_id> {
                    components->position2, components->sprite});
            for(auto ent: items)
            {
                auto* sprite = reg->get_component_or_null<CSprite>(
                        ent, components->sprite);
                auto* pos = reg->get_component_or_null<CPosition2>(
                        ent, components->position2);
                renderer->draw_sprite(
                        *sprite->texture,
                        GetSpriteRect(pos->pos, *sprite->texture));
            }
        }

        void
        register_callbacks(core::ecs::systems* systems) override
        {
            systems->sprite_drawer.add(this);
        }
    };


    void
    AddSystems(core::ecs::systems* systems, Sol* duk, Components* components)
    {
        systems->add_and_register(std::make_shared<SystemSpriteDraw>(components));
    }
}  // namespace euphoria::engine
