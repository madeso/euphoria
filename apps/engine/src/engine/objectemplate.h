#ifndef EUPHORIA_OBJECTTEMPLATE_H
#define EUPHORIA_OBJECTTEMPLATE_H

#include "core/ecs-id.h"
#include "core/sol_forward.h"

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

namespace euphoria::duk
{
    struct Context;
    struct Duk;
}  // namespace euphoria::duk

// struct FileSystem; // ?

namespace euphoria::render
{
    struct TextureCache;
}

namespace euphoria::engine
{
    struct DukRegistry;
    struct Components;

    struct ObjectCreationArgs
    {
        core::ecs::World* world;
        DukRegistry*      reg;
        Sol*  ctx;
        Sol*  duk;

        ObjectCreationArgs(
            core::ecs::World* aworld, DukRegistry* areg, Sol* actx, Sol* aduk);
};

    struct ComponentCreator
    {
    public:
        virtual ~ComponentCreator() = default;
        virtual void
        CreateComponent(const ObjectCreationArgs& args, core::ecs::EntityId id)
                = 0;
    };

    struct ObjectTemplate
    {
    public:
        core::ecs::EntityId
        CreateObject(const ObjectCreationArgs& args);

        std::vector<std::shared_ptr<ComponentCreator>> components;
    };

    struct ObjectCreator
    {
    public:
        std::map<std::string, std::shared_ptr<ObjectTemplate>> templates;

        ObjectTemplate*
        FindTemplate(const std::string& name);
    };

    void
    LoadTemplatesButOnlyNames(const game::Game& json, ObjectCreator* temp);

    void
    LoadTemplates(
            const game::Game&     json,
            ObjectCreator*        temp,
            DukRegistry*          reg,
            render::TextureCache* cache,
            Components*           components);
}  // namespace euphoria::engine

#endif  // EUPHORIA_OBJECTTEMPLATE_H
