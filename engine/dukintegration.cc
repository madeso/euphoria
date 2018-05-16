#include "engine/dukintegration.h"

#include <map>

#include "core/componentsystem.h"

#include "core/duk.h"
#include "engine/components.h"
#include "engine/input.h"
#include "engine/dukregistry.h"
#include "engine/objectemplate.h"

class DukUpdateSystem : public ComponentSystem, public ComponentSystemUpdate
{
  FunctionVar func;
  Duk*        duk;

 public:
  DukUpdateSystem(const std::string& name, FunctionVar f, Duk* d)
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

class DukSystems
{
 public:
  Systems* systems;
  Duk*     duk;

  DukSystems(Systems* s, Duk* d)
      : systems(s)
      , duk(d)
  {
  }

  void
  AddUpdate(const std::string& name, FunctionVar func)
  {
    systems->AddAndRegister(std::make_shared<DukUpdateSystem>(name, func, duk));
  }
};

struct DukIntegrationPimpl
{
  DukIntegrationPimpl(
      Systems*       sys,
      World*         world,
      Duk*           duk,
      ObjectCreator* creator,
      Components*    components)
      : systems(sys, duk)
      , registry(&world->reg, components)
      , input(duk->CreateGlobal("Input"))
      , world(world)
      , creator(creator)
      , components(components)
  {
  }

  void
  Integrate(Duk* duk)
  {
    duk->BindObject(
        "Systems",
        BindObject().AddFunction(
            "AddUpdate",
            Bind{}.bind<std::string, FunctionVar>(
                [&](Context* ctx, const std::string& name, FunctionVar func)
                    -> int {
                  func.StoreReference(ctx);
                  systems.AddUpdate(name, func);
                  return ctx->ReturnVoid();
                })));

    duk->BindObject(
        "Templates",
        BindObject().AddFunction(
            "Find",
            Bind{}.bind<std::string>(
                [&](Context* ctx, const std::string& name) -> int {
                  return ctx->ReturnFreeObject(creator->FindTemplate(name));
                })));
    duk->BindClass(
        "Template",
        BindClass<ObjectTemplate>().AddMethod(
            "Create",
            Bind{}.bind<ObjectTemplate>(
                [&, duk](Context* ctx, ObjectTemplate& t) -> int {
                  return ctx->Return(t.CreateObject(
                      ObjectCreationArgs{world, &registry, ctx, duk}));
                })));


    duk->BindObject(
        "Registry",
        BindObject()
            .AddFunction(
                "Entities",
                Bind{}.bind<std::vector<ComponentId>>(
                    [&](Context*                        ctx,
                        const std::vector<ComponentId>& types) -> int {
                      return ctx->ReturnArray(registry.EntityView(types));
                    }))
            .AddFunction("GetSpriteId", Bind{}.bind([&](Context* ctx) -> int {
              return ctx->Return(registry.components->sprite);
            }))
            .AddFunction(
                "DestroyEntity",
                Bind{}.bind<EntityId>(
                    [&](Context* ctx, EntityId entity) -> int {
                      registry.DestroyEntity(entity);
                      return ctx->ReturnVoid();
                    }))
            .AddFunction(
                "GetPosition2Id", Bind{}.bind([&](Context* ctx) -> int {
                  return ctx->Return(registry.components->position2);
                }))
            .AddFunction(
                "New",
                Bind{}
                    .bind<std::string>(
                        [&](Context* ctx, const std::string& name) -> int {
                          FunctionVar fv;
                          return ctx->Return(registry.CreateNewId(name, fv));
                        })
                    .bind<std::string, FunctionVar>(
                        [&](Context*           ctx,
                            const std::string& name,
                            FunctionVar        setup) -> int {
                          setup.StoreReference(ctx);
                          return ctx->Return(registry.CreateNewId(name, setup));
                        }))
            .AddFunction(
                "Get",
                Bind{}.bind<EntityId, ComponentId>(
                    [&](Context* ctx, EntityId ent, ComponentId comp) -> int {
                      return ctx->Return(registry.GetProperty(ent, comp));
                    }))
            .AddFunction(
                "Set",
                Bind{}.bind<EntityId, ComponentId, DukValue>(
                    [&](Context*    ctx,
                        EntityId    ent,
                        ComponentId comp,
                        DukValue    val) -> int {
                      registry.SetProperty(ent, comp, val);
                      return ctx->ReturnVoid();
                    }))
            .AddFunction(
                "GetPosition2",
                Bind{}.bind<ComponentId>(
                    [&](Context* ctx, ComponentId ent) -> int {
                      return ctx->ReturnFreeObject(
                          registry.GetComponentOrNull<CPosition2>(
                              ent, components->position2));
                    }))
            .AddFunction(
                "GetPosition2vec",
                Bind{}.bind<ComponentId>(
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
                Bind{}.bind<CPosition2>([](Context* ctx, CPosition2& p) -> int {
                  return ctx->ReturnObject(std::make_shared<vec2f>(p.pos));
                }))
            .AddMethod(
                "SetPos",
                Bind{}.bind<CPosition2, vec2f>(
                    [](Context* ctx, CPosition2& p, const vec2f& pos) -> int {
                      p.pos = pos;
                      return ctx->ReturnVoid();
                    }))
            .AddMethod(
                "GetPosRef",
                Bind{}.bind<CPosition2>([](Context* ctx, CPosition2& p) -> int {
                  return ctx->ReturnFreeObject(&p.pos);
                }))
            .AddProperty(
                "vec",
                Bind{}.bind<CPosition2>([](Context* ctx, CPosition2& p) -> int {
                  return ctx->ReturnFreeObject(&p.pos);
                }),
                Bind{}.bind<CPosition2, vec2f>(
                    [](Context* ctx, CPosition2& p, const vec2f& pos) -> int {
                      p.pos = pos;
                      return ctx->ReturnVoid();
                    })));

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
  DukValue       input;
  World*         world;
  ObjectCreator* creator;
  Components*    components;
};

DukIntegration::DukIntegration(
    Systems*       systems,
    World*         reg,
    Duk*           duk,
    ObjectCreator* creator,
    Components*    components)
{
  pimpl.reset(new DukIntegrationPimpl(systems, reg, duk, creator, components));
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
DukIntegration::BindKeys(Duk* duk, const Input& input)
{
  input.Set(duk, pimpl->input);
}
