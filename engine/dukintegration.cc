#include "engine/dukintegration.h"

#include <map>

#include "core/ecs-systems.h"
#include "core/random.h"
#include "core/log.h"

#include "duk/duk.h"
#include "duk/functionreference.h"
#include "duk/bindobject.h"
#include "duk/bindclass.h"

#include "engine/components.h"
#include "engine/input.h"
#include "engine/dukregistry.h"
#include "engine/objectemplate.h"
#include "engine/cameradata.h"

LOG_SPECIFY_DEFAULT_LOGGER("engine.duk.integration")

namespace euphoria::engine
{
    class DukUpdateSystem
        : public core::ecs::ComponentSystem
        , public core::ecs::ComponentSystemUpdate
    {
        duk::FunctionReference func;
        duk::Duk*              duk;

    public:
        DukUpdateSystem(
                const std::string&     name,
                duk::FunctionReference f,
                duk::Duk*              d)
            : ComponentSystem(name), func(f), duk(d)
        {}

        void
        Update(core::ecs::EntReg*, float dt) const override
        {
            ASSERT(duk);
            func.VoidCall(duk->AsContext(), dt);
        }

        void
        RegisterCallbacks(core::ecs::Systems* systems) override
        {
            systems->update.Add(this);
        }
    };

    class DukInitSystem
        : public core::ecs::ComponentSystem
        , public core::ecs::ComponentSystemInit
    {
        core::ecs::EntReg*                  reg;
        std::vector<core::ecs::ComponentId> types;
        duk::FunctionReference              func;
        duk::Duk*                           duk;

    public:
        DukInitSystem(
                const std::string&                         name,
                core::ecs::EntReg*                         r,
                const std::vector<core::ecs::ComponentId>& t,
                duk::FunctionReference                     f,
                duk::Duk*                                  d)
            : ComponentSystem(name), reg(r), types(t), func(f), duk(d)
        {}

        void
        OnAdd(core::ecs::EntityId entity) const override
        {
            ASSERT(duk);
            ASSERT(reg);
            ASSERT(!types.empty());

            for(const auto& type: types)
            {
                const bool has_component
                        = reg->GetComponent(entity, type) != nullptr;
                if(!has_component)
                {
                    return;
                }
            }
            func.VoidCall(duk->AsContext(), entity);
        }

        void
        RegisterCallbacks(core::ecs::Systems* systems) override
        {
            systems->init.Add(this);
        }
    };

    class DukSystems
    {
    public:
        core::ecs::Systems* systems;
        duk::Duk*           duk;

        DukSystems(core::ecs::Systems* s, duk::Duk* d) : systems(s), duk(d) {}

        void
        AddUpdate(const std::string& name, duk::FunctionReference func)
        {
            systems->AddAndRegister(
                    std::make_shared<DukUpdateSystem>(name, func, duk));
        }

        void
        AddInit(const std::string&                         name,
                core::ecs::EntReg*                         reg,
                const std::vector<core::ecs::ComponentId>& types,
                duk::FunctionReference                     func)
        {
            systems->AddAndRegister(std::make_shared<DukInitSystem>(
                    name, reg, types, func, duk));
        }
    };

    struct DukIntegrationPimpl
    {
        DukIntegrationPimpl(
                core::ecs::Systems* sys,
                core::ecs::World*   world,
                duk::Duk*           duk,
                ObjectCreator*      creator,
                Components*         components,
                CameraData*         cam)
            : systems(sys, duk)
            , registry(&world->reg, components)
            , input(duk->CreateGlobal("Input"))
            , world(world)
            , creator(creator)
            , components(components)
            , camera(cam)
        {}

        void
        Integrate(duk::Duk* duk)
        {
            using namespace duk;
            duk->BindObject(
                    "Systems",
                    BindObject()
                            .AddFunction(
                                    "AddUpdate",
                                    MakeBind<std::string, FunctionReference>(
                                            [&](Context*           ctx,
                                                const std::string& name,
                                                FunctionReference func) -> int {
                                                func.StoreReference(ctx);
                                                systems.AddUpdate(name, func);
                                                return ctx->ReturnVoid();
                                            }))

                            .AddFunction(
                                    "OnInit",
                                    MakeBind<
                                            std::string,
                                            std::vector<core::ecs::ComponentId>,
                                            FunctionReference>(
                                            [&](Context*           ctx,
                                                const std::string& name,
                                                const std::vector<
                                                        core::ecs::ComponentId>&
                                                                  types,
                                                FunctionReference func) -> int {
                                                func.StoreReference(ctx);
                                                systems.AddInit(
                                                        name,
                                                        &world->reg,
                                                        types,
                                                        func);
                                                return ctx->ReturnVoid();
                                            }))

            );

            duk->BindObject(
                    "Math",
                    BindObject().AddFunction(
                            "NewRandom", MakeBind<>([&](Context* ctx) -> int {
                                return ctx->ReturnObject(
                                        std::make_shared<core::Random>());
                            })));

            duk->BindObject(
                    "Templates",
                    BindObject().AddFunction(
                            "Find",
                            MakeBind<std::string>(
                                    [&](Context*           ctx,
                                        const std::string& name) -> int {
                                        return ctx->ReturnFreeObject(
                                                creator->FindTemplate(name));
                                    })));
            duk->BindClass(
                    "Template",
                    BindClass<ObjectTemplate>().AddMethod(
                            "Create",
                            MakeBind<ObjectTemplate>(
                                    [&, duk](Context* ctx, ObjectTemplate& t)
                                            -> int {
                                        return ctx->Return(t.CreateObject(
                                                ObjectCreationArgs {world,
                                                                    &registry,
                                                                    ctx,
                                                                    duk}));
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
                                    MakeBind<std::vector<
                                            core::ecs::ComponentId>>(
                                            [&](Context* ctx,
                                                const std::vector<
                                                        core::ecs::ComponentId>&
                                                        types) -> int {
                                                return ctx->ReturnArray(
                                                        registry.EntityView(
                                                                types));
                                            }))
                            .AddFunction(
                                    "GetSpriteId",
                                    MakeBind([&](Context* ctx) -> int {
                                        return ctx->Return(
                                                registry.components->sprite);
                                    }))
                            .AddFunction(
                                    "DestroyEntity",
                                    MakeBind<core::ecs::EntityId>(
                                            [&](Context*            ctx,
                                                core::ecs::EntityId entity)
                                                    -> int {
                                                registry.DestroyEntity(entity);
                                                return ctx->ReturnVoid();
                                            }))
                            .AddFunction(
                                    "GetPosition2Id",
                                    MakeBind([&](Context* ctx) -> int {
                                        return ctx->Return(
                                                registry.components->position2);
                                    }))
                            .AddFunction(
                                    "GetSpriteId",
                                    MakeBind([&](Context* ctx) -> int {
                                        return ctx->Return(
                                                registry.components->sprite);
                                    }))
                            .AddFunction(
                                    "New",
                                    MakeBind<
                                            std::string,
                                            Optional<FunctionReference>>(
                                            [&](Context*           ctx,
                                                const std::string& name,
                                                Optional<FunctionReference>
                                                        setup) -> int {
                                                if(setup)
                                                {
                                                    LOG_INFO("New id with ref");
                                                    ASSERT(setup->IsValid());
                                                    setup->StoreReference(ctx);
                                                }
                                                else
                                                {
                                                    LOG_INFO(
                                                            "New id with no ref");
                                                    ASSERT(!setup->IsValid());
                                                }
                                                return ctx->Return(
                                                        registry.CreateNewId(
                                                                name,
                                                                setup.value));
                                            }))
                            .AddFunction(
                                    "Get",
                                    MakeBind<
                                            core::ecs::EntityId,
                                            core::ecs::ComponentId>(
                                            [&](Context*               ctx,
                                                core::ecs::EntityId    ent,
                                                core::ecs::ComponentId comp)
                                                    -> int {
                                                return ctx->Return(
                                                        registry.GetProperty(
                                                                ent, comp));
                                            }))
                            .AddFunction(
                                    "Set",
                                    MakeBind<
                                            core::ecs::EntityId,
                                            core::ecs::ComponentId,
                                            ObjectReference>(
                                            [&](Context*               ctx,
                                                core::ecs::EntityId    ent,
                                                core::ecs::ComponentId comp,
                                                ObjectReference val) -> int {
                                                registry.SetProperty(
                                                        ent, comp, val);
                                                return ctx->ReturnVoid();
                                            }))
                            .AddFunction(
                                    "GetSprite",
                                    MakeBind<core::ecs::ComponentId>(
                                            [&](Context*               ctx,
                                                core::ecs::ComponentId ent)
                                                    -> int {
                                                return ctx->ReturnFreeObject(
                                                        registry.GetComponentOrNull<
                                                                CSprite>(
                                                                ent,
                                                                components
                                                                        ->sprite));
                                            }))
                            .AddFunction(
                                    "GetPosition2",
                                    MakeBind<core::ecs::ComponentId>(
                                            [&](Context*               ctx,
                                                core::ecs::ComponentId ent)
                                                    -> int {
                                                return ctx->ReturnFreeObject(
                                                        registry.GetComponentOrNull<
                                                                CPosition2>(
                                                                ent,
                                                                components
                                                                        ->position2));
                                            }))
                            .AddFunction(
                                    "GetPosition2vec",
                                    MakeBind<core::ecs::ComponentId>(
                                            [&](Context*               ctx,
                                                core::ecs::ComponentId ent)
                                                    -> int {
                                                auto c = registry.GetComponentOrNull<
                                                        CPosition2>(
                                                        ent,
                                                        components->position2);
                                                return ctx->ReturnFreeObject(
                                                        c == nullptr ? nullptr
                                                                     : &c->pos);
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
                                    MakeBind<CPosition2>(
                                            [](Context*    ctx,
                                               CPosition2& p) -> int {
                                                return ctx->ReturnObject(
                                                        std::make_shared<
                                                                core::vec2f>(
                                                                p.pos));
                                            }))
                            .AddMethod(
                                    "SetPos",
                                    MakeBind<CPosition2, core::vec2f>(
                                            [](Context*           ctx,
                                               CPosition2&        p,
                                               const core::vec2f& pos) -> int {
                                                p.pos = pos;
                                                return ctx->ReturnVoid();
                                            }))
                            .AddMethod(
                                    "GetPosRef",
                                    MakeBind<CPosition2>(
                                            [](Context*    ctx,
                                               CPosition2& p) -> int {
                                                return ctx->ReturnFreeObject(
                                                        &p.pos);
                                            }))
                            .AddProperty(
                                    "vec",
                                    MakeBind<CPosition2>(
                                            [](Context*    ctx,
                                               CPosition2& p) -> int {
                                                return ctx->ReturnFreeObject(
                                                        &p.pos);
                                            }),
                                    MakeBind<CPosition2, core::vec2f>(
                                            [](Context*           ctx,
                                               CPosition2&        p,
                                               const core::vec2f& pos) -> int {
                                                p.pos = pos;
                                                return ctx->ReturnVoid();
                                            })));

            duk->BindClass(
                    "CSprite",
                    BindClass<CSprite>().AddMethod(
                            "GetRect",
                            MakeBind<CSprite, CPosition2>(
                                    [](Context*          ctx,
                                       const CSprite&    sp,
                                       const CPosition2& p) -> int {
                                        return ctx->ReturnObject(
                                                std::make_shared<core::Rectf>(
                                                        GetSpriteRect(
                                                                p.pos,
                                                                *sp.texture)));
                                    })));

            duk->BindClass(
                    "Rectf",
                    BindClass<core::Rectf>()
                            .AddMethod(
                                    "Contains",
                                    MakeBind<core::Rectf, core::Rectf>(
                                            [](Context*           ctx,
                                               const core::Rectf& lhs,
                                               const core::Rectf& rhs) -> int {
                                                return ctx->ReturnBool(
                                                        lhs.ContainsExclusive(
                                                                rhs));
                                            }))
                            .AddMethod(
                                    "GetHeight",
                                    MakeBind<core::Rectf>(
                                            [](Context*           ctx,
                                               const core::Rectf& r) -> int {
                                                return ctx->ReturnNumber(
                                                        r.GetHeight());
                                            }))
                            .AddMethod(
                                    "GetWidth",
                                    MakeBind<core::Rectf>(
                                            [](Context*           ctx,
                                               const core::Rectf& r) -> int {
                                                return ctx->ReturnNumber(
                                                        r.GetWidth());
                                            })));

            duk->BindClass(
                    "Random",
                    BindClass<core::Random>()
                            .AddMethod(
                                    "NextFloat01",
                                    MakeBind<core::Random>(
                                            [](Context*      ctx,
                                               core::Random& rnd) -> int {
                                                return ctx->ReturnNumber(
                                                        rnd.NextFloat01());
                                            }))
                            .AddMethod(
                                    "NextRangeFloat",
                                    MakeBind<core::Random, float>(
                                            [](Context*      ctx,
                                               core::Random& rnd,
                                               float         f) -> int {
                                                return ctx->ReturnNumber(
                                                        rnd.NextRange(f));
                                            }))
                            .AddMethod(
                                    "NextBool",
                                    MakeBind<core::Random>(
                                            [](Context*      ctx,
                                               core::Random& rnd) -> int {
                                                return ctx->ReturnBool(
                                                        rnd.NextBool());
                                            }))
                            .AddMethod(
                                    "NextPoint2",
                                    MakeBind<core::Random, core::Rectf>(
                                            [](Context*           ctx,
                                               core::Random&      rnd,
                                               const core::Rectf& r) -> int {
                                                return ctx->ReturnObject(
                                                        std::make_shared<
                                                                core::vec2f>(
                                                                r.RandomPoint(
                                                                        &rnd)));
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

        DukSystems           systems;
        DukRegistry          registry;
        duk::ObjectReference input;
        core::ecs::World*    world;
        ObjectCreator*       creator;
        Components*          components;
        CameraData*          camera;
    };

    DukIntegration::DukIntegration(
            core::ecs::Systems* systems,
            core::ecs::World*   reg,
            duk::Duk*           duk,
            ObjectCreator*      creator,
            Components*         components,
            CameraData*         camera)
    {
        pimpl.reset(new DukIntegrationPimpl(
                systems, reg, duk, creator, components, camera));
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
}  // namespace euphoria::engine

TYPEID_SETUP_TYPE(euphoria::core::Random);
TYPEID_SETUP_TYPE(euphoria::engine::ObjectTemplate);
TYPEID_SETUP_TYPE(euphoria::core::Rect<float>);
