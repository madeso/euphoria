#include "engine/input.h"

#include "core/assert.h"

#include "core/sol.h"

namespace euphoria::engine
{
    BoundVar::BoundVar(const std::string& n, const core::Key& k)
        : name(n), state(0), last_state(0), key(k)
    {}

    void
BindBoundVar(Sol* duk)
{
  duk->lua.new_usertype<BoundVar>(
      "BoundVar",
      "state",
      sol::readonly(&BoundVar::state),
      "last_state",
      sol::readonly(&BoundVar::last_state));
}

void
Input::Bind(Sol* duk)
{
        BindBoundVar(duk);
}

    void
    Input::Add(std::shared_ptr<BoundVar> bind)
    {
        binds.push_back(bind);
    }

    void
    Input::SetKeyState(core::Key key, float state)
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
Input::Set(sol::table* container) const
{
  for(const auto& bind : binds)
  {
    (*container)[bind->name] = bind;
  }
}

    void
    Input::UpdateState()
    {
        for(auto& bind: binds)
        {
            bind->last_state = bind->state;
        }
    }
}  // namespace euphoria::engine
