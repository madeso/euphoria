#pragma once

#include <string>
#include <memory>
#include <vector>

#include "core/typeinfo.h"
#include "core/key.h"
#include "core/sol.h"


namespace euphoria::engine
{
    struct BoundVar
    {
        std::string name;
        float state;
        float last_state;
        core::Key key;

        BoundVar(const std::string& n, const core::Key& k);

        // todo(Gustav): figure out how to bind keys to this property
        // todo(Gustav): figure out how to handle just pressed
        // todo(Gustav): figure out how to handle player characters
        // todo(Gustav): figure out how to handle inputs other than keyboard
    };



    struct InputSystem
    {
    public:
        static void
        bind(LuaState* duk);

        void
        add(std::shared_ptr<BoundVar> bind);

        void
        set_key_state(core::Key key, float state);

        void
        set(sol::table* container) const;

        void
        update_state();

    private:
        std::vector<std::shared_ptr<BoundVar>> binds;
    };
}

TYPEID_SETUP_TYPE(euphoria::engine::BoundVar);

