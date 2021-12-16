#include "engine/dukintegration.h"

#include <map>
#include <functional>

#include "core/ecs-systems.h"
#include "core/random.h"
#include "core/log.h"
#include "core/sol.h"
#include "core/cint.h"

#include "engine/components.h"
#include "engine/input.h"
#include "engine/dukregistry.h"
#include "engine/objectemplate.h"
#include "engine/cameradata.h"


namespace euphoria::engine
{

    template<typename T>
    std::vector<T> get_vector(const sol::table& table)
    {
        std::vector<T> r;
        r.reserve(table.size());
        for(unsigned int i=0; i<table.size(); i+=1)
        {
            r.emplace_back(table[i+1]);
        }
        return r;
    }


    void report_first_error(const sol::protected_function_result& val, LuaState* ctx, const std::string& action, const std::string& where)
    {
        if (val.valid())
        {
            return;
        }

        const sol::error err = val;
        const auto message = fmt::format("Failed to {0} in {1}: {2}", action, where, err.what());
        LOG_ERROR("{0}", message);
        if (!ctx->has_error)
        {
            ctx->has_error = true;
            ctx->error = message;
        }
    }


    struct ScriptUpdateSystem
        : core::ecs::ComponentSystem
        , core::ecs::ComponentSystemUpdater
    {
        using UpdateFunction = sol::protected_function; // void(double)

        LuaState* duk;
        UpdateFunction func;

        ScriptUpdateSystem(const std::string& name, LuaState* d, UpdateFunction f)
            : ComponentSystem(name)
            , duk(d)
            , func(f)
        {
            ASSERT(func);
        }

        void
        update(core::ecs::Registry*, float dt) const override
        {
            auto result = func(core::c_float_to_double(dt));
            report_first_error(result, duk, "update", name);
        }

        void
        register_callbacks(core::ecs::Systems* systems) override
        {
            systems->updater.add(this);
        }
    };


    struct ScriptInitSystem
        : public core::ecs::ComponentSystem
        , public core::ecs::ComponentSystemInitializer
    {
    	using InitFunction = sol::protected_function; // void(entity_id)

        core::ecs::Registry* reg;
        LuaState* duk;
        std::vector<core::ecs::ComponentId> types;
        InitFunction func;


        ScriptInitSystem
        (
                const std::string& name,
                LuaState* d,
                core::ecs::Registry* r,
                const std::vector<core::ecs::ComponentId>& t,
                InitFunction f
        )
            : ComponentSystem(name)
            , reg(r)
            , duk(d)
            , types(t)
            , func(f)
        {
        }

        void
        on_add(core::ecs::EntityId entity) const override
        {
            ASSERT(reg);
            ASSERT(!types.empty());

            for(const auto& type: types)
            {
                const auto component = reg->get_component(entity, type);
                const bool has_component = component != nullptr;

                if(!has_component)
                {
                    return;
                }
            }
            auto result = func(entity);
            report_first_error(result, duk, "init", name);
        }

        void
        register_callbacks(core::ecs::Systems* systems) override
        {
            systems->initializer.add(this);
        }
    };


    struct ScriptSystems
    {
        core::ecs::Systems* systems;
        LuaState* duk;

        explicit ScriptSystems(core::ecs::Systems* s, LuaState* d)
            : systems(s)
            , duk(d)
        {
        }

        void
        add_update(const std::string& name, ScriptUpdateSystem::UpdateFunction func)
        {
            systems->add_and_register(std::make_shared<ScriptUpdateSystem>(name, duk, func));
        }

        void
        add_init
        (
            const std::string& name,
            core::ecs::Registry* reg,
            const std::vector<core::ecs::ComponentId>& types,
            ScriptInitSystem::InitFunction func
        )
        {
            systems->add_and_register(std::make_shared<ScriptInitSystem>(name, duk, reg, types, func));
        }
    };

    struct ScriptIntegrationPimpl
    {
        ScriptIntegrationPimpl
        (
                core::ecs::Systems* sys,
                core::ecs::World* world,
                LuaState* duk,
                ObjectCreator* creator,
                engine::Components* cc,
                CameraData* cam
        )
            : systems(sys, duk)
            , registry(&world->reg, cc)
            , input(duk->state["input"].get_or_create<sol::table>())
            , world(world)
            , creator(creator)
            , components(cc)
            , camera(cam)
        {
        }

        void
        integrate(LuaState* duk)
        {
            auto systems_table = duk->state["systems"].get_or_create<sol::table>();
            systems_table["add_update"] = [&](const std::string& name, sol::function func)
            {
                systems.add_update(name, func);
            };
            systems_table["on_init"] = [&]
            (
                const std::string& name,
                sol::table types,
                sol::function func
            )
            {
                auto vtypes = get_vector<core::ecs::ComponentId>(types);
                systems.add_init(name, &world->reg, vtypes, func);
            };

            auto math_table = duk->state["math"].get_or_create<sol::table>();
            math_table["new_random"] = [&]() { return std::make_shared<core::Random>(); };

            auto templates_table = duk->state["templates"].get_or_create<sol::table>();
            templates_table["find"] = [&](const std::string& name)
            {
                return creator->find_template(name);
            };

            auto template_type = duk->state.new_usertype<ObjectTemplate>("template");
            template_type["create"] = [&, duk](ObjectTemplate* t)
            {
                return t->create_object(ObjectCreationArguments{world, &registry, duk, duk});
            };

            auto camera_table = duk->state["camera"].get_or_create<sol::table>();
            camera_table["get_rect"] = [&]() { return &camera->screen; };

            duk->state.new_usertype<CustomArguments>
            (
                "custom_arguments",
                "get_number", [](const CustomArguments& args, const std::string& name) -> double
                {
                    const auto f = args.numbers.find(name);
                    if(f == args.numbers.end())
                    {
                        return 0.0;
                    }
                    else
                    {
                        return core::c_float_to_double(f->second);
                    }
                }
            );

            auto registry_table = duk->state["registry"].get_or_create<sol::table>();
            registry_table["entities"] = [&](sol::table types)
            {
                auto vec = get_vector<core::ecs::ComponentId>(types);
                if(vec.empty())
                {
                    throw std::runtime_error("no entities specified");
                }
                return registry.entity_view(vec);
            };
            registry_table["get_sprite_id"] = [&]()
            {
                return registry.components->sprite;
            };
            registry_table["destroy_entity"] = [&](core::ecs::EntityId entity)
            {
                registry.destroy_entity(entity);
            };
            registry_table["get_position2_id"] = [&]()
            {
                LOG_INFO("Getting position 2d");
                return registry.components->position2;
            };
            registry_table["get_sprite_id"] = [&]()
            {
                return registry.components->sprite;
            };
            registry_table["create"] = sol::overload
            (
                [&](const std::string& name, sol::function setup)
                {
                    return registry.create_new_id(name, setup);
                },
                [&](const std::string& name)
                {
                    return registry.create_new_id(name);
                }
            );
            registry_table["get"] = [&](core::ecs::EntityId ent, core::ecs::ComponentId comp)
            {
                return registry.get_property(ent, comp);
            };
            registry_table["set"] = [&](core::ecs::EntityId ent, core::ecs::ComponentId comp, sol::table val)
            {
                registry.set_property(ent, comp, val);
            };
            registry_table["get_sprite"] = [&](core::ecs::ComponentId ent)
            {
                return registry.get_component_or_null<ComponentSprite>(ent, components->sprite);
            };
            registry_table["get_position2"] = [&](core::ecs::ComponentId ent)
            {
                return registry.get_component_or_null<ComponentPosition2>(ent, components->position2);
            };
            registry_table["get_position2_vec"] = [&](core::ecs::ComponentId ent)
            {
                auto* c = registry.get_component_or_null<ComponentPosition2>(ent, components->position2);
                return c == nullptr ? nullptr : &c->pos;
            };

            duk->state.new_usertype<ComponentPosition2>
            (
                "component_position2",
                "vec", sol::property
                (
                    [](const ComponentPosition2& p)
                    {
                        return p.pos;
                    },
                    [](ComponentPosition2& p, const core::Vec2f& np)
                    {
                        p.pos = np;
                    }
                )
            );

            duk->state.new_usertype<ComponentSprite>
            (
                "component_sprite",
                "get_rect", [](const ComponentSprite& sp, const ComponentPosition2& p)
                {
                    return get_sprite_rect(p.pos, *sp.texture);
                }
            );

            auto rect_type = duk->state.new_usertype<core::Rectf>("rectf", sol::no_constructor);
            rect_type["contains"] = sol::overload
            (
                [](const core::Rectf& r, const core::Rectf& rr) -> bool
                {
                    return r.contains_exclusive(rr);
                },
                [](const core::Rectf& r, const core::Vec2f& p) -> bool
                {
                    return r.contains_exclusive(p);
                },
                [](const core::Rectf& r, double x, double y) -> bool
                {
                    return r.contains_exclusive(core::c_double_to_float(x), core::c_double_to_float(y));
                }
            );
            rect_type["get_height"] = [](const core::Rectf& r) -> double { return core::c_float_to_double(r.get_height()); };
            rect_type["get_width"] = [](const core::Rectf& r) -> double { return core::c_float_to_double(r.get_width()); };

            auto random_type = duk->state.new_usertype<core::Random>("random");
            random_type["next_float01"] = [](core::Random& r) -> double { return core::c_float_to_double(r.get_next_float01()); };
            random_type["next_range_float"] = [](core::Random& r, double f) -> double
            {
                return core::c_float_to_double(get_random_in_range(&r, core::c_double_to_float(f)));
            };
            random_type["next_bool"] = &core::Random::get_next_bool;
            random_type["next_point2"] = [](core::Random& r, core::Rectf& rect) -> core::Vec2f
            {
                return rect.get_random_point(&r);
            };
        }

        ScriptSystems systems;
        ScriptRegistry registry;
        sol::table input;
        core::ecs::World* world;
        ObjectCreator* creator;
        engine::Components* components;
        CameraData* camera;
    };


    ScriptIntegration::ScriptIntegration
    (
            core::ecs::Systems* systems,
            core::ecs::World* reg,
            LuaState* duk,
            ObjectCreator* creator,
            Components* components,
            CameraData* camera
    )
    {
        pimpl = std::make_unique<ScriptIntegrationPimpl>
        (
            systems,
            reg,
            duk,
            creator,
            components,
            camera
        );
        pimpl->integrate(duk);
    }


    ScriptIntegration::~ScriptIntegration()
    {
        clear();
    }


    void
    ScriptIntegration::clear()
    {
        pimpl.reset();
    }


    ScriptRegistry&
    ScriptIntegration::get_registry()
    {
        ASSERT(pimpl);
        return pimpl->registry;
    }


    void
    ScriptIntegration::bind_keys(const InputSystem &input)
    {
        input.set(&pimpl->input);
    }
}

TYPEID_SETUP_TYPE(euphoria::core::Random);
TYPEID_SETUP_TYPE(euphoria::engine::ObjectTemplate);
TYPEID_SETUP_TYPE(euphoria::core::Rect<float>);
