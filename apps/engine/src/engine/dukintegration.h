#pragma once

#include <memory>

#include "core/sol.h"
#include "core/noncopyable.h"

namespace euphoria::engine
{
    struct InputSystem;
    struct ScriptRegistry;
    struct ObjectCreator;
    struct ScriptIntegrationPimpl;
    struct CameraData;
    struct Components;
    struct Systems;
    struct World;
    struct Systems;


    struct ScriptIntegration
    {
    public:
        ScriptIntegration
        (
            Systems* systems,
            World* reg,
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
