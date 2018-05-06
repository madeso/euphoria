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

ObjectCreationArgs::ObjectCreationArgs(World* aworld, DukRegistry* areg)
    : world(aworld)
    , reg(areg)
{
}

////////////////////////////////////////////////////////////////////////////////

class PositionComponentCreator : public ComponentCreator
{
 public:
  vec2f p;

  explicit PositionComponentCreator(const vec2f& pp)
      : p(pp)
  {
  }

  static std::shared_ptr<PositionComponentCreator>
  Create(const game::vec2f& p)
  {
    return std::make_shared<PositionComponentCreator>(vec2f{p.x(), p.y()});
  }

  void
  CreateComponent(const ObjectCreationArgs& args, EntityId ent) override
  {
    args.world->reg.assign<CPosition2>(ent).pos = p;
  }
};

class SpriteComponentCreator : public ComponentCreator
{
 public:
  std::shared_ptr<Texture2d> texture;

  static std::shared_ptr<SpriteComponentCreator>
  Create(const game::Sprite& sprite, TextureCache* cache)
  {
    auto ptr     = std::make_shared<SpriteComponentCreator>();
    ptr->texture = cache->GetTexture(sprite.path());
    return ptr;
  }

  void
  CreateComponent(const ObjectCreationArgs& args, EntityId ent) override
  {
    args.world->reg.assign<CSprite>(ent).texture = texture;
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
    // todo: come up with a better default value
    // perhaps some convert/setup function...
    args.reg->SetProperty(ent, comp, DukValue());
  }
};

////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<ComponentCreator>
CreateCreator(
    const game::Component& comp, DukRegistry* reg, TextureCache* cache)
{
  if(comp.has_position())
  {
    return PositionComponentCreator::Create(comp.position());
  }
  else if(comp.has_sprite())
  {
    return SpriteComponentCreator::Create(comp.sprite(), cache);
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
    TextureCache*         cache)
{
  for(const auto& comp : ct.components())
  {
    auto c = CreateCreator(comp, reg, cache);
    if(c != nullptr)
    {
      ot->components.emplace_back(c);
    }
  }
}

void
ObjectTemplate::CreateObject(const ObjectCreationArgs& args)
{
  auto ent = args.world->reg.create();
  for(auto c : components)
  {
    c->CreateComponent(args, ent);
  }
}

////////////////////////////////////////////////////////////////////////////////

void
LoadTemplatesButOnlyNames(const game::Game& json, ObjectCreator* temp)
{
  for(const auto& t : json.templates())
  {
    auto o = std::make_shared<ObjectTemplate>();
    temp->templates.insert(
        std::make_pair(t.name(), o));
  }
}

void
LoadTemplates(
    const game::Game& json,
    ObjectCreator*    temp,
    DukRegistry*      reg,
    TextureCache*     cache)
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

    LoadObjectTemplate(o.get(), t, reg, cache);
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
