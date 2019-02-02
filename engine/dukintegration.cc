#include "engine/dukintegration.h"

#include <map>
#include <functional>

#include "core/componentsystem.h"
#include "core/random.h"
#include "core/log.h"
#include "core/sol.h"

#include "engine/components.h"
#include "engine/input.h"
#include "engine/dukregistry.h"
#include "engine/objectemplate.h"
#include "engine/cameradata.h"

LOG_SPECIFY_DEFAULT_LOGGER("engine.duk.integration")

struct DukUpdateSystem : public ComponentSystem, public ComponentSystemUpdate
{
  using UpdateFunction = std::function<void(float)>;

  UpdateFunction func;

  DukUpdateSystem(const std::string& name, UpdateFunction f)
      : ComponentSystem(name)
      , func(f)
  {
    ASSERT(func);
  }

  void
  Update(EntReg* reg, float dt) const override
  {
    func(dt);
  }

  void
  RegisterCallbacks(Systems* systems) override
  {
    systems->update.Add(this);
  }
};

struct DukInitSystem : public ComponentSystem, public ComponentSystemInit
{
  using InitFunction = std::function<void(EntityId)>;

  EntReg*                  reg;
  std::vector<ComponentId> types;
  InitFunction             func;

  DukInitSystem(
      const std::string&              name,
      EntReg*                         r,
      const std::vector<ComponentId>& t,
      InitFunction                    f)
      : ComponentSystem(name)
      , reg(r)
      , types(t)
      , func(f)
  {
  }

  void
  OnAdd(EntityId entity) const override
  {
    ASSERT(reg);
    ASSERT(!types.empty());

    for(const auto& type : types)
    {
      const bool has_component = reg->GetComponent(entity, type) != nullptr;
      if(!has_component)
      {
        return;
      }
    }
    func(entity);
  }

  void
  RegisterCallbacks(Systems* systems) override
  {
    systems->init.Add(this);
  }
};

struct DukSystems
{
  Systems* systems;

  explicit DukSystems(Systems* s)
      : systems(s)
  {
  }

  void
  AddUpdate(const std::string& name, DukUpdateSystem::UpdateFunction func)
  {
    systems->AddAndRegister(std::make_shared<DukUpdateSystem>(name, func));
  }

  void
  AddInit(
      const std::string&              name,
      EntReg*                         reg,
      const std::vector<ComponentId>& types,
      DukInitSystem::InitFunction     func)
  {
    systems->AddAndRegister(
        std::make_shared<DukInitSystem>(name, reg, types, func));
  }
};

TYPEID_SETUP_TYPE(Random);
TYPEID_SETUP_TYPE(ObjectTemplate);
TYPEID_SETUP_TYPE(Rect<float>);

struct DukIntegrationPimpl
{
  DukIntegrationPimpl(
      Systems*       sys,
      World*         world,
      sol::state*    duk,
      ObjectCreator* creator,
      Components*    components,
      CameraData*    cam)
      : systems(sys)
      , registry(&world->reg, components)
      , input((*duk)["Input"].get_or_create<sol::table>())
      , world(world)
      , creator(creator)
      , components(components)
      , camera(cam)
  {
  }

  void
  Integrate(sol::state* duk)
  {
    auto systems_table         = (*duk)["Systems"].get_or_create<sol::table>();
    systems_table["AddUpdate"] = [&](
        const std::string& name, sol::function func) {
      systems.AddUpdate(name, func);
    };
    systems_table["OnInit"] = [&](
        const std::string&              name,
        const std::vector<ComponentId>& types,
        sol::function                   func) {
      LOG_INFO("OnInit");
      systems.AddInit(name, &world->reg, types, func);
      LOG_INFO("OnInit done");
    };

    auto math_table         = (*duk)["Math"].get_or_create<sol::table>();
    math_table["NewRandom"] = [&]() { return std::make_shared<Random>(); };

    auto templates_table    = (*duk)["Templates"].get_or_create<sol::table>();
    templates_table["Find"] = [&](const std::string& name) {
      return creator->FindTemplate(name);
    };

    duk->new_usertype<ObjectTemplate>("Template");
    (*duk)["Template"]["Create"] = [&, duk](ObjectTemplate* t) {
      return t->CreateObject(ObjectCreationArgs{world, &registry, duk, duk});
    };

    auto camera_table       = (*duk)["Camera"].get_or_create<sol::table>();
    camera_table["GetRect"] = [&]() { return &camera->screen; };

    duk->new_usertype<CustomArguments>(
        "CustomArguments",
        "GetNumber",
        [](const CustomArguments& args, const std::string& name) -> float {
          const auto f = args.numbers.find(name);
          if(f == args.numbers.end())
          {
            return 0;
          }
          else
          {
            return f->second;
          }
        });

    auto registry_table        = (*duk)["Registry"].get_or_create<sol::table>();
    registry_table["Entities"] = [&](const std::vector<ComponentId>& types) {
      return registry.EntityView(types);
    };
    registry_table["GetSpriteId"] = [&]() {
      return registry.components->sprite;
    };
    registry_table["DestroyEntity"] = [&](EntityId entity) {
      registry.DestroyEntity(entity);
    };
    registry_table["GetPosition2Id"] = [&]() {
      return registry.components->position2;
    };
    registry_table["GetSpriteId"] = [&]() {
      return registry.components->sprite;
    };
    registry_table["New"] = [&](const std::string& name, sol::function setup) {
      return registry.CreateNewId(name, setup);
    };
    registry_table["Get"] = [&](EntityId ent, ComponentId comp) {
      return registry.GetProperty(ent, comp);
    };
    registry_table["Set"] = [&](
        EntityId ent, ComponentId comp, sol::table val) {
      registry.SetProperty(ent, comp, val);
    };
    registry_table["GetSprite"] = [&](ComponentId ent) {
      return registry.GetComponentOrNull<CSprite>(ent, components->sprite);
    };
    registry_table["GetPosition2"] = [&](ComponentId ent) {
      return registry.GetComponentOrNull<CPosition2>(
          ent, components->position2);
    };
    registry_table["GetPosition2vec"] = [&](ComponentId ent) {
      auto c =
          registry.GetComponentOrNull<CPosition2>(ent, components->position2);
      return c == nullptr ? nullptr : &c->pos;
    };

    duk->new_usertype<CPosition2>("CPosition2", "vec", &CPosition2::pos);

    duk->new_usertype<CSprite>(
        "CSprite", "GetRect", [](const CSprite& sp, const CPosition2& p) {
          return GetSpriteRect(p.pos, *sp.texture);
        });

    /*
    duk->new_usertype<Rectf>(
        "Rectf",
        sol::no_constructor,
        "Contains",
        &Rectf::ContainsExclusive,
        "GetHeight",
        &Rectf::GetHeight,
        "GetWidth",
        &Rectf::GetWidth);
    */

    duk->new_usertype<Random>(
        "Random", "NextFloat01", &Random::NextFloat01
        // "NextRangeFloat", &Random::NextRange<float>,
        // "NextBool", &Random::NextBool,
        // "NextPoint", &Random::NextPoint
        );
  }

  DukSystems     systems;
  DukRegistry    registry;
  sol::table     input;
  World*         world;
  ObjectCreator* creator;
  Components*    components;
  CameraData*    camera;
};

DukIntegration::DukIntegration(
    Systems*       systems,
    World*         reg,
    sol::state*    duk,
    ObjectCreator* creator,
    Components*    components,
    CameraData*    camera)
{
  pimpl.reset(
      new DukIntegrationPimpl(systems, reg, duk, creator, components, camera));
  pimpl->Integrate(duk);
}

DukIntegration::~DukIntegration()
{
  Clear();
}

void
DukIntegration::Clear()
{
  pimpl.reset();
}

DukRegistry&
DukIntegration::Registry()
{
  ASSERT(pimpl);
  return pimpl->registry;
}

void
DukIntegration::BindKeys(sol::state* duk, const Input& input)
{
  input.Set(&pimpl->input);
}
