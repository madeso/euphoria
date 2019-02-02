#include "engine/systems.h"

#include "core/componentsystem.h"

#include "render/texture.h"
#include "render/spriterender.h"
#include "render/texturecache.h"

#include "engine/components.h"

struct SystemSpriteDraw : public ComponentSystem,
                          public ComponentSystemSpriteDraw
{
  Components* components;

  explicit SystemSpriteDraw(Components* c)
      : ComponentSystem("sprite draw")
      , components(c)
  {
  }

  void
  Draw(EntReg* reg, SpriteRenderer* renderer) const override
  {
    const auto items = reg->View(
        std::vector<ComponentId>{components->position2, components->sprite});
    for(auto ent : items)
    {
      auto* sprite = reg->GetComponentOrNull<CSprite>(ent, components->sprite);
      auto* pos =
          reg->GetComponentOrNull<CPosition2>(ent, components->position2);
      renderer->DrawSprite(
          *sprite->texture, GetSpriteRect(pos->pos, *sprite->texture));
    }
  }

  void
  RegisterCallbacks(Systems* systems) override
  {
    systems->spriteDraw.Add(this);
  }
};


void
AddSystems(Systems* systems, sol::state* duk, Components* components)
{
  systems->AddAndRegister(std::make_shared<SystemSpriteDraw>(components));
}
