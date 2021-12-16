#pragma once

#include <memory>

#include "core/sol.h"
#include "core/noncopyable.h"

namespace euphoria::core::ecs
{
    struct Systems;
    struct World;
}

namespace euphoria::engine
{
    struct InputSystem;
    struct ScriptRegistry;
    struct ObjectCreator;
    struct ScriptIntegrationPimpl;
    struct CameraData;
    struct Components;


    struct ScriptIntegration
    {
    public:
        ScriptIntegration
        (
                core::ecs::Systems* systems,
                core::ecs::World* reg,
                LuaState* duk,
                ObjectCreator* creator,
                Components* components,
                CameraData* camera
        );
        ~ScriptIntegration();

        NONCOPYABLE(ScriptIntegration);

        void
        clear();

        ScriptRegistry&
        get_registry();

        void
        bind_keys(const InputSystem& input);

    private:
        std::unique_ptr<ScriptIntegrationPimpl> pimpl;
    };
}
