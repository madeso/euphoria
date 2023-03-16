#pragma once

#include <memory>

#include "core/sol.h"


namespace euphoria::runner
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

        ScriptIntegration(const ScriptIntegration& other) = delete;
        void operator=(const ScriptIntegration&) = delete;
        ScriptIntegration(ScriptIntegration&& other) = delete;
        void operator=(ScriptIntegration&&) = delete;

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
