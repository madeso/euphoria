#ifndef EUPHORIA_OBJECTTEMPLATE_H
#define EUPHORIA_OBJECTTEMPLATE_H

#include "core/ecs-id.h"

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace game
{
    class Game;
}

namespace euphoria::core::ecs
{
    class World;
}

namespace euphoria::duk
{
    class Context;
    class Duk;
}  // namespace euphoria::duk

namespace euphoria::render
{
    class TextureCache;
}

namespace euphoria::engine
{
    class DukRegistry;
    struct Components;

    struct ObjectCreationArgs
    {
        core::ecs::World*  world;
        DukRegistry*  reg;
        duk::Context* ctx;
        duk::Duk*     duk;

        ObjectCreationArgs(
                core::ecs::World*  aworld,
                DukRegistry*  areg,
                duk::Context* actx,
                duk::Duk*     aduk);
    };

    class ComponentCreator
    {
        public:
        virtual ~ComponentCreator() = default;
        virtual void
        CreateComponent(const ObjectCreationArgs& args, core::ecs::EntityId id)
                = 0;
    };

    class ObjectTemplate
    {
        public:
        core::ecs::EntityId
        CreateObject(const ObjectCreationArgs& args);

        std::vector<std::shared_ptr<ComponentCreator>> components;
    };

    class ObjectCreator
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
