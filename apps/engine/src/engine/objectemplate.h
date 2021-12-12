#pragma once

#include "core/ecs-id.h"
#include "core/sol_forward.h"
#include "core/noncopyable.h"

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace game
{
    struct Game;
}

namespace euphoria::core::ecs
{
    struct World;
}

namespace euphoria::render
{
    struct texture_cache;
}

namespace euphoria::engine
{
    struct script_registry;
    struct components;

    struct object_creation_arguments
    {
        core::ecs::World* world;
        script_registry* reg;
        LuaState* ctx;
        LuaState* duk;

        object_creation_arguments
        (
                core::ecs::World* aworld,
                script_registry* areg,
                LuaState* actx,
                LuaState* aduk
        );
};

    struct component_creator
    {
        component_creator() = default;
        virtual ~component_creator() = default;

        NONCOPYABLE(component_creator);

        virtual void
        create_component(const object_creation_arguments& args, core::ecs::entity_id id) = 0;
    };

    struct object_template
    {
        core::ecs::entity_id
        create_object(const object_creation_arguments& args);

        std::vector<std::shared_ptr<component_creator>> components;
    };

    struct object_creator
    {
        std::map<std::string, std::shared_ptr<object_template>> templates;

        object_template*
        find_template(const std::string& name);
    };

    void
    load_templates_but_only_names(const game::Game& json, object_creator* temp);

    void
    load_templates
    (
        const game::Game& json,
        object_creator* temp,
        script_registry* reg,
        render::texture_cache* cache,
        components* components
    );
}
