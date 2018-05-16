#include "engine/objectemplate.h"

#include "core/stdutils.h"
#include "core/stringmerger.h"

#include "core/log.h"
#include "core/proto.h"

#include "engine/components.h"
#include "engine/dukregistry.h"

#include "game.pb.h"

LOG_SPECIFY_DEFAULT_LOGGER("engine.templates")

////////////////////////////////////////////////////////////////////////////////

ObjectCreationArgs::ObjectCreationArgs(
    World* aworld, DukRegistry* areg, Context* actx, Duk* aduk)
    : world(aworld)
    , reg(areg)
    , ctx(actx)
    , duk(aduk)
{
}

////////////////////////////////////////////////////////////////////////////////

class PositionComponentCreator : public ComponentCreator
{
 public:
  vec2f       p;
  Components* components;

  PositionComponentCreator(const vec2f& pp, Components* components)
      : p(pp)
      , components(components)
  {
  }

  static std::shared_ptr<PositionComponentCreator>
  Create(const game::vec2f& p, Components* components)
  {
    return std::make_shared<PositionComponentCreator>(
        vec2f{p.x(), p.y()}, components);
  }

  void
  CreateComponent(const ObjectCreationArgs& args, EntityId ent) override
  {
    auto c = std::make_shared<CPosition2>();
    c->pos = p;
    args.world->reg.AddComponent(ent, components->position2, c);
  }
};

class SpriteComponentCreator : public ComponentCreator
{
 public:
  std::shared_ptr<Texture2d> texture;
  Components*                components;

  explicit SpriteComponentCreator(Components* c)
      : components(c)
  {
  }

  static std::shared_ptr<SpriteComponentCreator>
  Create(
      const game::Sprite& sprite, TextureCache* cache, Components* components)
  {
    auto ptr     = std::make_shared<SpriteComponentCreator>(components);
    ptr->texture = cache->GetTexture(sprite.path());
    return ptr;
  }

  void
  CreateComponent(const ObjectCreationArgs& args, EntityId ent) override
  {
    auto c     = std::make_shared<CSprite>();
    c->texture = texture;
    args.world->reg.AddComponent(ent, components->sprite, c);
  }
};

class CustomComponentCreator : public ComponentCreator
{
 public:
  ComponentId comp;

  static std::shared_ptr<CustomComponentCreator>
  Create(ComponentId id)
  {
    auto ptr  = std::make_shared<CustomComponentCreator>();
    ptr->comp = id;
    return ptr;
  }

  void
  CreateComponent(const ObjectCreationArgs& args, EntityId ent) override
  {
    auto val = args.reg->CreateComponent(comp, args.ctx);
    val.StoreReference(args.duk);
    args.reg->SetProperty(ent, comp, val);
  }
};

////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<ComponentCreator>
CreateCreator(
    const game::Component& comp,
    DukRegistry*           reg,
    TextureCache*          cache,
    Components*            components)
{
  if(comp.has_position())
  {
    return PositionComponentCreator::Create(comp.position(), components);
  }
  else if(comp.has_sprite())
  {
    return SpriteComponentCreator::Create(comp.sprite(), cache, components);
  }
  else if(comp.has_custom())
  {
    const auto& s = comp.custom();
    ComponentId id;
    if(reg->GetCustomComponentByName(s.name(), &id))
    {
      return CustomComponentCreator::Create(id);
    }
    else
    {
      LOG_ERROR("No custom component named " << s.name() << " was added.");
      return nullptr;
    }
  }

  LOG_ERROR("No component found, bug!!!");
  return nullptr;
}

void
LoadObjectTemplate(
    ObjectTemplate*       ot,
    const game::Template& ct,
    DukRegistry*          reg,
    TextureCache*         cache,
    Components*           components)
{
  for(const auto& comp : ct.components())
  {
    auto c = CreateCreator(comp, reg, cache, components);
    if(c != nullptr)
    {
      ot->components.emplace_back(c);
    }
  }
}

EntityId
ObjectTemplate::CreateObject(const ObjectCreationArgs& args)
{
  auto ent = args.world->reg.Create();
  for(const auto& c : components)
  {
    c->CreateComponent(args, ent);
  }

  return ent;
}

////////////////////////////////////////////////////////////////////////////////

void
LoadTemplatesButOnlyNames(const game::Game& json, ObjectCreator* temp)
{
  for(const auto& t : json.templates())
  {
    auto o = std::make_shared<ObjectTemplate>();
    temp->templates.insert(std::make_pair(t.name(), o));
  }
}

void
LoadTemplates(
    const game::Game& json,
    ObjectCreator*    temp,
    DukRegistry*      reg,
    TextureCache*     cache,
    Components*       components)
{
  for(const auto& t : json.templates())
  {
    auto o = std::make_shared<ObjectTemplate>();

    auto fr = temp->templates.find(t.name());
    if(fr == temp->templates.end())
    {
      temp->templates.insert(std::make_pair(t.name(), o));
    }
    else
    {
      o = fr->second;
    }

    LoadObjectTemplate(o.get(), t, reg, cache, components);
  }
}

ObjectTemplate*
ObjectCreator::FindTemplate(const std::string& name)
{
  auto result = templates.find(name);
  if(result == templates.end())
  {
    LOG_ERROR(
        "Failed to find template named "
        << name
        << ", could be "
        << StringMerger::EnglishOr().Generate(Keys(templates))
        << ".");
    return nullptr;
  }

  return result->second.get();
}

////////////////////////////////////////////////////////////////////////////////
