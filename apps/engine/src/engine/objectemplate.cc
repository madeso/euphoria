#include "engine/objectemplate.h"

#include "core/stdutils.h"
#include "core/stringmerger.h"
#include "core/vfs_path.h"
#include "core/log.h"
#include "core/proto.h"

#include "engine/components.h"
#include "engine/dukregistry.h"

#include "gaf_game.h"


namespace euphoria::engine
{
    ////////////////////////////////////////////////////////////////////////////////

    ObjectCreationArguments::ObjectCreationArguments
    (
            core::ecs::World* aworld,
            ScriptRegistry* areg,
            LuaState* actx,
            LuaState* aduk
    )
        : world(aworld)
        , reg(areg)
        , ctx(actx)
        , duk(aduk)
    {
    }

    ////////////////////////////////////////////////////////////////////////////////

    struct PositionComponentCreator : ComponentCreator
    {
        core::Vec2f p;
        engine::Components* components;

        PositionComponentCreator(const core::Vec2f& pp, engine::Components* cs)
            : p(pp)
            , components(cs)
        {
        }

        [[nodiscard]]
        static
        std::shared_ptr<PositionComponentCreator>
        create(const game::vec2f& p, engine::Components* components)
        {
            return std::make_shared<PositionComponentCreator>(core::Vec2f {p.x, p.y}, components);
        }

        void
        create_component(const ObjectCreationArguments& args, core::ecs::EntityId ent) override
        {
            auto c = std::make_shared<ComponentPosition2>();
            c->pos = p;
            args.world->reg.add_component_to_entity(ent, components->position2, c);
        }
    };

    struct SpriteComponentCreator : public ComponentCreator
    {
    public:
        std::shared_ptr<render::Texture2> texture;
        engine::Components* components;

        explicit SpriteComponentCreator(engine::Components* c) : components(c) {}

        [[nodiscard]]
        static
        std::shared_ptr<SpriteComponentCreator>
        create
        (
            const game::Sprite& sprite,
            render::TextureCache* cache,
            engine::Components* components
        )
        {
            auto ptr = std::make_shared<SpriteComponentCreator>(components);
            ptr->texture = cache->get_texture(core::vfs::FilePath::from_script(sprite.path));
            return ptr;
        }

        void
        create_component(const ObjectCreationArguments& args, core::ecs::EntityId ent) override
        {
            auto c = std::make_shared<ComponentSprite>();
            c->texture = texture;
            args.world->reg.add_component_to_entity(ent, components->sprite, c);
        }
    };


    struct CustomComponentCreator : public ComponentCreator
    {
    public:
        core::ecs::ComponentId comp;
        CustomArguments arguments;

        explicit CustomComponentCreator(core::ecs::ComponentId id)
            : comp(id)
        {
        }

        [[nodiscard]]
        static
        std::shared_ptr<CustomComponentCreator>
        create
        (
            const std::string& name,
            core::ecs::ComponentId id,
            const std::vector<game::Var>& arguments
        )
        {
            auto ptr = std::make_shared<CustomComponentCreator>(id);
            for(const auto& a: arguments)
            {
                if(a.number != nullptr)
                {
                    ptr->arguments.numbers[a.name] = a.number->value;
                }
                else
                {
                    LOG_ERROR
                    (
                        "Invalid type for custom argument {0} for type {1}",
                        a.name,
                        name
                    );
                }
            }
            return ptr;
        }

        void
        create_component(const ObjectCreationArguments& args, core::ecs::EntityId ent) override
        {
            auto val = args.reg->create_component(comp, args.ctx, arguments);
            args.reg->set_property(ent, comp, val);
        }
    };


    ////////////////////////////////////////////////////////////////////////////////


    std::shared_ptr<ComponentCreator>
    create_creator
    (
        const game::Component& comp,
        ScriptRegistry* reg,
        render::TextureCache* cache,
        Components* components
    )
    {
        if(comp.position)
        {
            return PositionComponentCreator::create(*comp.position, components);
        }
        else if(comp.sprite)
        {
            return SpriteComponentCreator::create
            (
                *comp.sprite,
                cache,
                components
            );
        }
        else if(comp.custom)
        {
            const auto& s = *comp.custom;
            if(const auto id = reg->get_custom_component_by_name(s.name); id)
            {
                return CustomComponentCreator::create(s.name, *id, s.arguments);
            }
            else
            {
                LOG_ERROR
                (
                    "No custom component named {0} was added: {1}",
                    s.name,
                    id.get_error()
                );
                return nullptr;
            }
        }

        LOG_ERROR("No component found, bug!!!");
        return nullptr;
    }


    void
    load_object_template
    (
        ObjectTemplate* ot,
        const game::Template& ct,
        ScriptRegistry* reg,
        render::TextureCache* cache,
        Components* components
    )
    {
        for(const auto& comp: ct.components)
        {
            auto c = create_creator(comp, reg, cache, components);
            if(c != nullptr)
            {
                ot->components.emplace_back(c);
            }
        }
    }


    core::ecs::EntityId
    ObjectTemplate::create_object(const ObjectCreationArguments& args)
    {
        auto ent = args.world->reg.create_new_entity();
        for(const auto& c: components)
        {
            c->create_component(args, ent);
        }

        // todo(Gustav): run init function here
        args.world->reg.post_create(ent);

        return ent;
    }


    ////////////////////////////////////////////////////////////////////////////////


    void
    load_templates_but_only_names(const game::Game& json, ObjectCreator* temp)
    {
        for(const auto& t: json.templates)
        {
            auto o = std::make_shared<ObjectTemplate>();
            temp->templates.insert(std::make_pair(t.name, o));
        }
    }


    void
    load_templates
    (
        const game::Game& json,
        ObjectCreator* temp,
        ScriptRegistry* reg,
        render::TextureCache* cache,
        Components* components
    )
    {
        for(const auto& t: json.templates)
        {
            auto o = std::make_shared<ObjectTemplate>();

            auto fr = temp->templates.find(t.name);
            if(fr == temp->templates.end())
            {
                temp->templates.insert(std::make_pair(t.name, o));
            }
            else
            {
                o = fr->second;
            }

            load_object_template(o.get(), t, reg, cache, components);
        }
    }


    ObjectTemplate*
    ObjectCreator::find_template(const std::string& name)
    {
        auto result = templates.find(name);
        if(result == templates.end())
        {
            LOG_ERROR
            (
                "Failed to find template named {0}, could be {1}.",
                name,
                core::string_mergers::english_or.merge(core::get_keys(templates))
            );
            return nullptr;
        }

        return result->second.get();
    }

}
