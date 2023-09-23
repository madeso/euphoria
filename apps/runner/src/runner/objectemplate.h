#pragma once

#include <memory>
#include <map>

#include "core/ecs.id.h"
#include "core/sol_forward.h"


namespace eu::files::game
{
    struct Game;
}

namespace eu::render
{
    struct TextureCache;
}

namespace eu::runner
{
    struct ScriptRegistry;
    struct Components;
    struct World;

    struct ObjectCreationArguments
    {
        World* world;
        ScriptRegistry* reg;
        LuaState* ctx;
        LuaState* duk;

        ObjectCreationArguments
        (
            World* aworld,
            ScriptRegistry* areg,
            LuaState* actx,
            LuaState* aduk
        );
};

    struct ComponentCreator
    {
        ComponentCreator() = default;
        virtual ~ComponentCreator() = default;

        ComponentCreator(const ComponentCreator& other) = delete;
        void operator=(const ComponentCreator&) = delete;
        ComponentCreator(ComponentCreator&& other) = delete;
        void operator=(ComponentCreator&&) = delete;

        virtual void
        create_component(const ObjectCreationArguments& args, core::ecs::EntityHandle id) = 0;
    };

    struct ObjectTemplate
    {
        core::ecs::EntityHandle
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
    load_templates_but_only_names(const files::game::Game& json, ObjectCreator* temp);

    void
    load_templates
    (
        const files::game::Game& json,
        ObjectCreator* temp,
        ScriptRegistry* reg,
        render::TextureCache* cache,
        Components* components
    );
}
