#include "engine/systems.h"

#include "core/ecs-systems.h"

#include "render/texture.h"
#include "render/spriterender.h"
#include "render/texturecache.h"

#include "engine/components.h"

namespace euphoria::engine
{
    struct SystemSpriteDraw
        : public core::ecs::ComponentSystem
        , public core::ecs::ComponentSystemSpriteDraw
    {
        Components* components;

        explicit SystemSpriteDraw(Components* c)
            : ComponentSystem("sprite draw"), components(c)
        {}

        void
        Draw(core::ecs::EntReg*      reg,
             render::SpriteRenderer* renderer) const override
        {
            const auto items = reg->View(std::vector<core::ecs::ComponentId> {
                    components->position2, components->sprite});
            for(auto ent: items)
            {
                auto* sprite = reg->GetComponentOrNull<CSprite>(
                        ent, components->sprite);
                auto* pos = reg->GetComponentOrNull<CPosition2>(
                        ent, components->position2);
                renderer->DrawSprite(
                        *sprite->texture,
                        GetSpriteRect(pos->pos, *sprite->texture));
            }
        }

        void
        RegisterCallbacks(core::ecs::Systems* systems) override
        {
            systems->spriteDraw.Add(this);
        }
    };


    void
    AddSystems(
            core::ecs::Systems* systems,
            duk::Duk*           duk,
            Components*         components)
    {
        systems->AddAndRegister(std::make_shared<SystemSpriteDraw>(components));
    }
}  // namespace euphoria::engine
