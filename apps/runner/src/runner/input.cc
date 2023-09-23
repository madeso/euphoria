#include "runner/input.h"

#include "assert/assert.h"
#include "base/cint.h"
#include "core/sol.h"

namespace eu::runner
{
    BoundVar::BoundVar(const std::string& n, const core::Key& k)
        : name(n), state(0), last_state(0), key(k)
    {
    }

    void
    bind_bound_var(LuaState* duk)
    {
        duk->state.new_usertype<BoundVar>
        (
            "bound_var",
            "state",
            sol::property
            (
                [](const BoundVar& v) -> double { return c_float_to_double(v.state); }
            ),
            "last_state",
            sol::property
            (
                [](const BoundVar& v) -> double { return c_float_to_double(v.last_state); }
            )
        );
    }


    void
    InputSystem::bind(LuaState* duk)
    {
        bind_bound_var(duk);
    }


    void
    InputSystem::add(std::shared_ptr<BoundVar> bind)
    {
        binds.push_back(bind);
    }


    void
    InputSystem::set_key_state(core::Key key, float state)
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
    InputSystem::set(sol::table* container) const
    {
        for(const auto& bind : binds)
        {
            (*container)[bind->name] = bind;
        }
    }

    void
    InputSystem::update_state()
    {
        for(auto& bind: binds)
        {
            bind->last_state = bind->state;
        }
    }
}
