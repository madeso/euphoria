#pragma once

#include <string>
#include <memory>
#include <vector>
#include <core/typeinfo.h>

#include "core/key.h"

#include "core/sol.h"


namespace euphoria::engine
{
    struct bound_var
    {
        std::string name;
        float state;
        float last_state;
        core::key key;

        bound_var(const std::string& n, const core::key& k);

        // todo(Gustav): figure out how to bind keys to this property
        // todo(Gustav): figure out how to handle just pressed
        // todo(Gustav): figure out how to handle player characters
        // todo(Gustav): figure out how to handle inputs other than keyboard
    };
    
    

    struct input_system
    {
    public:
        static void
        bind(Sol* duk);

        void
        add(std::shared_ptr<bound_var> bind);

        void
        set_key_state(core::key key, float state);

        void
        set(sol::table* container) const;

        void
        update_state();

    private:
        std::vector<std::shared_ptr<bound_var>> binds;
    };
}

TYPEID_SETUP_TYPE(euphoria::engine::bound_var);

