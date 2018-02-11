#include "engine/components.h"

#include <memory>

#include "core/componentsystem.h"
#include "core/vec2.h"
#include "core/log.h"

#include "render/texture.h"
#include "render/spriterender.h"
#include "render/texturecache.h"

#include "world.pb.h"
#include "core/proto.h"

LOG_SPECIFY_DEFAULT_LOGGER("engine.components")

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
LoadWorld(
    FileSystem* fs, World* world, TextureCache* cache, const std::string& path)
{
  world::World json;
  const auto   err = LoadProtoJson(fs, &json, path);
  if(!err.empty())
  {
    LOG_ERROR("Failed to load world components from " << path << ": " << err);
  }

  for(const auto& obj : json.objects())
  {
    auto ent = world->reg.create();
    for(const auto& comp : obj.components())
    {
      if(comp.has_position())
      {
        const auto& p                          = comp.position();
        world->reg.assign<CPosition2>(ent).pos = vec2f{p.x(), p.y()};
      }
      else if(comp.has_sprite())
      {
        const auto& s                           = comp.sprite();
        world->reg.assign<CSprite>(ent).texture = cache->GetTexture(s.path());
      }
    }
  }
}
