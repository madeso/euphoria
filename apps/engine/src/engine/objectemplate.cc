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

    object_creation_arguments::object_creation_arguments
    (
        core::ecs::world* aworld,
        script_registry* areg,
        Sol* actx,
        Sol* aduk
    )
        : world(aworld)
        , reg(areg)
        , ctx(actx)
        , duk(aduk)
    {
    }

    ////////////////////////////////////////////////////////////////////////////////

    struct position_component_creator : component_creator
    {
        core::vec2f p;
        engine::components* components;

        position_component_creator(const core::vec2f& pp, engine::components* components)
            : p(pp)
            , components(components)
        {
        }

        [[nodiscard]]
        static
        std::shared_ptr<position_component_creator>
        create(const game::vec2f& p, engine::components* components)
        {
            return std::make_shared<position_component_creator>(core::vec2f {p.x, p.y}, components);
        }

        void
        create_component(const object_creation_arguments& args, core::ecs::entity_id ent) override
        {
            auto c = std::make_shared<component_position2>();
            c->pos = p;
            args.world->reg.add_component_to_entity(ent, components->position2, c);
        }
    };

    struct sprite_component_creator : public component_creator
    {
    public:
        std::shared_ptr<render::texture2d> texture;
        engine::components* components;

        explicit sprite_component_creator(engine::components* c) : components(c) {}

        [[nodiscard]]
        static
        std::shared_ptr<sprite_component_creator>
        create
        (
            const game::Sprite& sprite,
            render::texture_cache* cache,
            engine::components* components
        )
        {
            auto ptr = std::make_shared<sprite_component_creator>(components);
            ptr->texture = cache->get_texture(core::vfs::file_path::from_script(sprite.path));
            return ptr;
        }

        void
        create_component(const object_creation_arguments& args, core::ecs::entity_id ent) override
        {
            auto c = std::make_shared<component_sprite>();
            c->texture = texture;
            args.world->reg.add_component_to_entity(ent, components->sprite, c);
        }
    };


    struct custom_component_creator : public component_creator
    {
    public:
        core::ecs::component_id comp;
        custom_arguments arguments;

        [[nodiscard]]
        static
        std::shared_ptr<custom_component_creator>
        create
        (
            const std::string& name,
            core::ecs::component_id id,
            const std::vector<game::Var>& arguments
        )
        {
            auto ptr = std::make_shared<custom_component_creator>();
            ptr->comp = id;
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
        create_component(const object_creation_arguments& args, core::ecs::entity_id ent) override
        {
            auto val = args.reg->create_component(comp, args.ctx, arguments);
            args.reg->set_property(ent, comp, val);
        }
    };


    ////////////////////////////////////////////////////////////////////////////////


    std::shared_ptr<component_creator>
    create_creator
    (
        const game::Component& comp,
        script_registry* reg,
        render::texture_cache* cache,
        components* components
    )
    {
        if(comp.position)
        {
            return position_component_creator::create(*comp.position, components);
        }
        else if(comp.sprite)
        {
            return sprite_component_creator::create
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
                return custom_component_creator::create(s.name, *id, s.arguments);
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
        object_template* ot,
        const game::Template& ct,
        script_registry* reg,
        render::texture_cache* cache,
        components* components
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


    core::ecs::entity_id
    object_template::create_object(const object_creation_arguments& args)
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
    load_templates_but_only_names(const game::Game& json, object_creator* temp)
    {
        for(const auto& t: json.templates)
        {
            auto o = std::make_shared<object_template>();
            temp->templates.insert(std::make_pair(t.name, o));
        }
    }


    void
    load_templates
    (
        const game::Game& json,
        object_creator* temp,
        script_registry* reg,
        render::texture_cache* cache,
        components* components
    )
    {
        for(const auto& t: json.templates)
        {
            auto o = std::make_shared<object_template>();

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


    object_template*
    object_creator::find_template(const std::string& name)
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
