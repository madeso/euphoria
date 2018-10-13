#include "engine/dukintegration.h"

#include <map>

#include "core/componentsystem.h"
#include "core/random.h"
#include "core/log.h"

#include "duk/duk.h"
#include "duk/function.h"
#include "duk/bindobject.h"
#include "duk/bindclass.h"

#include "engine/components.h"
#include "engine/input.h"
#include "engine/dukregistry.h"
#include "engine/objectemplate.h"
#include "engine/cameradata.h"

LOG_SPECIFY_DEFAULT_LOGGER("engine.duk.integration")

class DukUpdateSystem : public ComponentSystem, public ComponentSystemUpdate
{
  duk::FunctionVar func;
  duk::Duk*        duk;

 public:
  DukUpdateSystem(const std::string& name, duk::FunctionVar f, duk::Duk* d)
      : ComponentSystem(name)
      , func(f)
      , duk(d)
  {
  }

  void
  Update(EntReg* reg, float dt) const override
  {
    ASSERT(duk);
    func.VoidCall(duk->AsContext(), dt);
  }

  void
  RegisterCallbacks(Systems* systems) override
  {
    systems->update.Add(this);
  }
};

class DukInitSystem : public ComponentSystem, public ComponentSystemInit
{
  EntReg*                  reg;
  std::vector<ComponentId> types;
  duk::FunctionVar         func;
  duk::Duk*                duk;

 public:
  DukInitSystem(
      const std::string&              name,
      EntReg*                         r,
      const std::vector<ComponentId>& t,
      duk::FunctionVar                f,
      duk::Duk*                       d)
      : ComponentSystem(name)
      , reg(r)
      , types(t)
      , func(f)
      , duk(d)
  {
  }

  void
  OnAdd(EntityId entity) const override
  {
    ASSERT(duk);
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
    func.VoidCall(duk->AsContext(), entity);
  }

  void
  RegisterCallbacks(Systems* systems) override
  {
    systems->init.Add(this);
  }
};

class DukSystems
{
 public:
  Systems*  systems;
  duk::Duk* duk;

  DukSystems(Systems* s, duk::Duk* d)
      : systems(s)
      , duk(d)
  {
  }

  void
  AddUpdate(const std::string& name, duk::FunctionVar func)
  {
    systems->AddAndRegister(std::make_shared<DukUpdateSystem>(name, func, duk));
  }

  void
  AddInit(
      const std::string&              name,
      EntReg*                         reg,
      const std::vector<ComponentId>& types,
      duk::FunctionVar                func)
  {
    systems->AddAndRegister(
        std::make_shared<DukInitSystem>(name, reg, types, func, duk));
  }
};

struct DukIntegrationPimpl
{
  DukIntegrationPimpl(
      Systems*       sys,
      World*         world,
      duk::Duk*      duk,
      ObjectCreator* creator,
      Components*    components,
      CameraData*    cam)
      : systems(sys, duk)
      , registry(&world->reg, components)
      , input(duk->CreateGlobal("Input"))
      , world(world)
      , creator(creator)
      , components(components)
      , camera(cam)
  {
  }

  void
  Integrate(duk::Duk* duk)
  {
    using namespace duk;
    duk->BindObject(
        "Systems",
        BindObject()
            .AddFunction(
                "AddUpdate",
                MakeBind<std::string, FunctionVar>(
                    [&](Context* ctx, const std::string& name, FunctionVar func)
                        -> int {
                      func.StoreReference(ctx);
                      systems.AddUpdate(name, func);
                      return ctx->ReturnVoid();
                    }))

            .AddFunction(
                "OnInit",
                MakeBind<std::string, std::vector<ComponentId>, FunctionVar>(
                    [&](Context*                        ctx,
                        const std::string&              name,
                        const std::vector<ComponentId>& types,
                        FunctionVar                     func) -> int {
                      func.StoreReference(ctx);
                      systems.AddInit(name, &world->reg, types, func);
                      return ctx->ReturnVoid();
                    }))

            );

    duk->BindObject(
        "Math",
        BindObject().AddFunction(
            "NewRandom", MakeBind<>([&](Context* ctx) -> int {
              return ctx->ReturnObject(std::make_shared<Random>());
            })));

    duk->BindObject(
        "Templates",
        BindObject().AddFunction(
            "Find",
            MakeBind<std::string>(
                [&](Context* ctx, const std::string& name) -> int {
                  return ctx->ReturnFreeObject(creator->FindTemplate(name));
                })));
    duk->BindClass(
        "Template",
        BindClass<ObjectTemplate>().AddMethod(
            "Create",
            MakeBind<ObjectTemplate>(
                [&, duk](Context* ctx, ObjectTemplate& t) -> int {
                  return ctx->Return(t.CreateObject(
                      ObjectCreationArgs{world, &registry, ctx, duk}));
                })));

    duk->BindObject(
        "Camera",
        BindObject().AddFunction(
            "GetRect", MakeBind<>([&](Context* ctx) -> int {
              return ctx->ReturnFreeObject(&camera->screen);
            })));

    duk->BindClass(
        "CustomArguments",
        BindClass<CustomArguments>().AddMethod(
            "GetNumber",
            MakeBind<CustomArguments, std::string>(
                [](Context*               ctx,
                   const CustomArguments& args,
                   const std::string&     name) -> int {
                  const auto f = args.numbers.find(name);
                  if(f == args.numbers.end())
                  {
                    return ctx->ReturnNumber(0);
                  }
                  else
                  {
                    return ctx->ReturnNumber(f->second);
                  }
                })));

    duk->BindObject(
        "Registry",
        BindObject()
            .AddFunction(
                "Entities",
                MakeBind<std::vector<ComponentId>>(
                    [&](Context*                        ctx,
                        const std::vector<ComponentId>& types) -> int {
                      return ctx->ReturnArray(registry.EntityView(types));
                    }))
            .AddFunction("GetSpriteId", MakeBind([&](Context* ctx) -> int {
                           return ctx->Return(registry.components->sprite);
                         }))
            .AddFunction(
                "DestroyEntity",
                MakeBind<EntityId>([&](Context* ctx, EntityId entity) -> int {
                  registry.DestroyEntity(entity);
                  return ctx->ReturnVoid();
                }))
            .AddFunction("GetPosition2Id", MakeBind([&](Context* ctx) -> int {
                           return ctx->Return(registry.components->position2);
                         }))
            .AddFunction("GetSpriteId", MakeBind([&](Context* ctx) -> int {
                           return ctx->Return(registry.components->sprite);
                         }))
            .AddFunction(
                "New",
                MakeBind<std::string, Optional<FunctionVar>>(
                    [&](Context*              ctx,
                        const std::string&    name,
                        Optional<FunctionVar> setup) -> int {
                      if(setup)
                      {
                        LOG_INFO("New id with ref");
                        ASSERT(setup->IsValid());
                        setup->StoreReference(ctx);
                      }
                      else
                      {
                        LOG_INFO("New id with no ref");
                        ASSERT(!setup->IsValid());
                      }
                      return ctx->Return(
                          registry.CreateNewId(name, setup.value));
                    }))
            .AddFunction(
                "Get",
                MakeBind<EntityId, ComponentId>(
                    [&](Context* ctx, EntityId ent, ComponentId comp) -> int {
                      return ctx->Return(registry.GetProperty(ent, comp));
                    }))
            .AddFunction(
                "Set",
                MakeBind<EntityId, ComponentId, DukValue>(
                    [&](Context*    ctx,
                        EntityId    ent,
                        ComponentId comp,
                        DukValue    val) -> int {
                      registry.SetProperty(ent, comp, val);
                      return ctx->ReturnVoid();
                    }))
            .AddFunction(
                "GetSprite",
                MakeBind<ComponentId>(
                    [&](Context* ctx, ComponentId ent) -> int {
                      return ctx->ReturnFreeObject(
                          registry.GetComponentOrNull<CSprite>(
                              ent, components->sprite));
                    }))
            .AddFunction(
                "GetPosition2",
                MakeBind<ComponentId>(
                    [&](Context* ctx, ComponentId ent) -> int {
                      return ctx->ReturnFreeObject(
                          registry.GetComponentOrNull<CPosition2>(
                              ent, components->position2));
                    }))
            .AddFunction(
                "GetPosition2vec",
                MakeBind<ComponentId>(
                    [&](Context* ctx, ComponentId ent) -> int {
                      auto c = registry.GetComponentOrNull<CPosition2>(
                          ent, components->position2);
                      return ctx->ReturnFreeObject(
                          c == nullptr ? nullptr : &c->pos);
                    })));

    //    dukglue_register_global(duk->ctx, &registry, "Registry");
    //    dukglue_register_method(duk->ctx, &DukRegistry::entities, "Entities");
    //    dukglue_register_method(duk->ctx, &DukRegistry::getSpriteId,
    //    "GetSpriteId");
    //    dukglue_register_method(
    //        duk->ctx, &DukRegistry::getPosition2dId, "GetPosition2Id");
    //
    //    dukglue_register_method(duk->ctx, &DukRegistry::CreateNewId, "New");
    //    dukglue_register_method(duk->ctx, &DukRegistry::GetProperty, "Get");
    //    dukglue_register_method(duk->ctx, &DukRegistry::SetProperty, "Set");
    //
    //    dukglue_register_method(
    //        duk->ctx, &DukRegistry::GetComponentOrNull<CPosition2>,
    //        "GetPosition2");

    duk->BindClass(
        "CPosition2",
        BindClass<CPosition2>()
            .AddMethod(
                "GetPos",
                MakeBind<CPosition2>([](Context* ctx, CPosition2& p) -> int {
                  return ctx->ReturnObject(std::make_shared<vec2f>(p.pos));
                }))
            .AddMethod(
                "SetPos",
                MakeBind<CPosition2, vec2f>(
                    [](Context* ctx, CPosition2& p, const vec2f& pos) -> int {
                      p.pos = pos;
                      return ctx->ReturnVoid();
                    }))
            .AddMethod(
                "GetPosRef",
                MakeBind<CPosition2>([](Context* ctx, CPosition2& p) -> int {
                  return ctx->ReturnFreeObject(&p.pos);
                }))
            .AddProperty(
                "vec",
                MakeBind<CPosition2>([](Context* ctx, CPosition2& p) -> int {
                  return ctx->ReturnFreeObject(&p.pos);
                }),
                MakeBind<CPosition2, vec2f>(
                    [](Context* ctx, CPosition2& p, const vec2f& pos) -> int {
                      p.pos = pos;
                      return ctx->ReturnVoid();
                    })));

    duk->BindClass(
        "CSprite",
        BindClass<CSprite>().AddMethod(
            "GetRect",
            MakeBind<CSprite, CPosition2>(
                [](Context* ctx, const CSprite& sp, const CPosition2& p)
                    -> int {
                  return ctx->ReturnObject(std::make_shared<Rectf>(
                      GetSpriteRect(p.pos, *sp.texture)));
                })));

    duk->BindClass(
        "Rectf",
        BindClass<Rectf>()
            .AddMethod(
                "Contains",
                MakeBind<Rectf, Rectf>(
                    [](Context* ctx, const Rectf& lhs, const Rectf& rhs)
                        -> int {
                      return ctx->ReturnBool(lhs.ContainsExclusive(rhs));
                    }))
            .AddMethod(
                "GetHeight",
                MakeBind<Rectf>([](Context* ctx, const Rectf& r) -> int {
                  return ctx->ReturnNumber(r.GetHeight());
                }))
            .AddMethod(
                "GetWidth",
                MakeBind<Rectf>([](Context* ctx, const Rectf& r) -> int {
                  return ctx->ReturnNumber(r.GetWidth());
                })));

    duk->BindClass(
        "Random",
        BindClass<Random>()
            .AddMethod(
                "NextFloat01",
                MakeBind<Random>([](Context* ctx, Random& rnd) -> int {
                  return ctx->ReturnNumber(rnd.NextFloat01());
                }))
            .AddMethod(
                "NextRangeFloat",
                MakeBind<Random, float>(
                    [](Context* ctx, Random& rnd, float f) -> int {
                      return ctx->ReturnNumber(rnd.NextRange(f));
                    }))
            .AddMethod(
                "NextBool",
                MakeBind<Random>([](Context* ctx, Random& rnd) -> int {
                  return ctx->ReturnBool(rnd.NextBool());
                }))
            .AddMethod(
                "NextPoint2",
                MakeBind<Random, Rectf>(
                    [](Context* ctx, Random& rnd, const Rectf& r) -> int {
                      return ctx->ReturnObject(
                          std::make_shared<vec2f>(rnd.NextPoint(r)));
                    }))

            );

    //    dukglue_register_method(duk->ctx, &CPosition2::GetPosition, "GetPos");
    //    dukglue_register_method(duk->ctx, &CPosition2::SetPosition, "SetPos");
    //    dukglue_register_method(duk->ctx, &CPosition2::GetPositionRef,
    //    "GetPosRef");
    //    dukglue_register_property(
    //        duk->ctx, &CPosition2::GetPosition, &CPosition2::SetPosition,
    //        "vec");
  }

  DukSystems     systems;
  DukRegistry    registry;
  duk::DukValue  input;
  World*         world;
  ObjectCreator* creator;
  Components*    components;
  CameraData*    camera;
};

DukIntegration::DukIntegration(
    Systems*       systems,
    World*         reg,
    duk::Duk*      duk,
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
DukIntegration::BindKeys(duk::Duk* duk, const Input& input)
{
  input.Set(duk, pimpl->input);
}
