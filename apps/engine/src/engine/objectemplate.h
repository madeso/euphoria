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
    struct TextureCache;
}

namespace euphoria::engine
{
    struct ScriptRegistry;
    struct Components;

    struct ObjectCreationArguments
    {
        core::ecs::World* world;
        ScriptRegistry* reg;
        LuaState* ctx;
        LuaState* duk;

        ObjectCreationArguments
        (
                core::ecs::World* aworld,
                ScriptRegistry* areg,
                LuaState* actx,
                LuaState* aduk
        );
};

    struct ComponentCreator
    {
        ComponentCreator() = default;
        virtual ~ComponentCreator() = default;

        NONCOPYABLE(ComponentCreator);

        virtual void
        create_component(const ObjectCreationArguments& args, core::ecs::EntityId id) = 0;
    };

    struct ObjectTemplate
    {
        core::ecs::EntityId
        create_object(const ObjectCreationArguments& args);

        std::vector<std::shared_ptr<ComponentCreator>> components;
    };

    struct ObjectCreator
    {
        std::map<std::string, std::shared_ptr<ObjectTemplate>> templates;

        ObjectTemplate*
        find_template(const std::string& name);
    };

    void
    load_templates_but_only_names(const game::Game& json, ObjectCreator* temp);

    void
    load_templates
    (
        const game::Game& json,
        ObjectCreator* temp,
        ScriptRegistry* reg,
        render::TextureCache* cache,
        Components* components
    );
}
