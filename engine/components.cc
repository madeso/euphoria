#include "engine/components.h"

#include <memory>

#include "core/componentsystem.h"
#include "core/vec2.h"

#include "render/texture.h"
#include "render/spriterender.h"
#include "render/texturecache.h"

struct CPosition2
{
  CPosition2()
      : pos(0, 0)
  {
  }
  vec2f pos;
};

struct CSprite
{
  std::shared_ptr<Texture2d> texture;
};

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
        [renderer](auto entity, auto pos, auto sprite) {
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

struct SystemMoveUp : public ComponentSystem, public ComponentSystemUpdate
{
  SystemMoveUp()
      : ComponentSystem("move up")
  {
  }

  void
  Update(EntReg* reg, float dt) const override
  {
    reg->view<CPosition2>().each(
        [dt](auto entity, auto& pos) { pos.pos.y += dt * 20; });
  }

  void
  RegisterCallbacks(Systems* systems) override
  {
    systems->update.Add(this);
  }
};

void
AddSystems(Systems* systems)
{
  systems->AddAndRegister(std::make_shared<SystemSpriteDraw>());
  systems->AddAndRegister(std::make_shared<SystemMoveUp>());
}

void
LoadWorld(World* world, const std::string& path, TextureCache* cache)
{
  auto ent                               = world->reg.create();
  world->reg.assign<CPosition2>(ent).pos = vec2f{400, 20};
  world->reg.assign<CSprite>(ent).texture =
      cache->GetTexture("playerShip1_blue.png");
}
