#include "engine/input.h"

#include "dukglue/dukglue.h"

BoundVar::BoundVar(const std::string& n, const Key& k)
    : name(n)
    , state(0)
    , last_state(0)
    , key(k)
{
}

void
Input::Add(std::shared_ptr<BoundVar> bind)
{
  binds.push_back(bind);
}

void
Input::SetKeyState(Key key, float state)
{
  // todo: move state to another class, and fix this loop
  for(auto bind : binds)
  {
    if(bind->key == key)
    {
      bind->state = state;
    }
  }
}

void
Input::Set(Duk* duk, DukValue container) const
{
  dukglue_push(duk->ctx, container);

  for(const auto& bind : binds)
  {
    duk_push_number(duk->ctx, bind->state);
    auto rc = duk_put_prop_string(duk->ctx, -2, bind->name.c_str());
    // todo: handle error
  }

  duk_pop(duk->ctx);

  // todo: validate duk stack
}

void
Input::UpdateState()
{
  for(auto& bind : binds)
  {
    bind->last_state = bind->state;
  }
}
