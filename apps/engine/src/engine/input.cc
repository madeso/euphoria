#include "engine/input.h"

#include "core/assert.h"
#include "core/cint.h"
#include "core/sol.h"

namespace euphoria::engine
{
    bound_var::bound_var(const std::string& n, const core::key& k)
        : name(n), state(0), last_state(0), key(k)
    {
    }

    void
    bind_bound_var(Sol* duk)
    {
        duk->lua.new_usertype<bound_var>
        (
            "bound_var",
            "state",
            sol::property
            (
                [](const bound_var& v) -> double { return core::c_float_to_double(v.state); }
            ),
            "last_state",
            sol::property
            (
                [](const bound_var& v) -> double { return core::c_float_to_double(v.last_state); }
            )
        );
    }


    void
    input_system::bind(Sol* duk)
    {
        bind_bound_var(duk);
    }


    void
    input_system::add(std::shared_ptr<bound_var> bind)
    {
        binds.push_back(bind);
    }


    void
    input_system::set_key_state(core::key key, float state)
    {
        // todo(Gustav): move state to another class, and fix this loop
        for(const auto& bind: binds)
        {
            if(bind->key == key)
            {
                bind->state = state;
            }
        }
    }

    void
    input_system::set(sol::table* container) const
    {
        for(const auto& bind : binds)
        {
            (*container)[bind->name] = bind;
        }
    }

    void
    input_system::update_state()
    {
        for(auto& bind: binds)
        {
            bind->last_state = bind->state;
        }
    }
}
