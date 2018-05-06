#include "engine/systems.h"

#include "core/componentsystem.h"

#include "render/texture.h"
#include "render/spriterender.h"
#include "render/texturecache.h"

#include "engine/components.h"

struct SystemSpriteDraw : public ComponentSystem,
                          public ComponentSystemSpriteDraw
{
  SystemSpriteDraw()
      : ComponentSystem("sprite draw")
  {
  }

  void
  Draw(EntReg* reg, SpriteRenderer* renderer) const override
  {
    reg->view<CPosition2, CSprite>().each(
        [renderer](auto entity, const auto& pos, const auto& sprite) {
          // LOG_INFO("Draw callback " << pos.pos);
          renderer->DrawSprite(*sprite.texture, pos.pos);
        });
  }

  void
  RegisterCallbacks(Systems* systems) override
  {
    systems->spriteDraw.Add(this);
  }
};


void
AddSystems(Systems* systems, Duk* duk)
{
  systems->AddAndRegister(std::make_shared<SystemSpriteDraw>());
}
