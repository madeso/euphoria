#include "engine/input.h"

#include "dukglue/dukglue.h"

BoundVar::BoundVar(const std::string& n, const Key& k)
    : name(n)
    , value(0)
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
      bind->value = state;
    }
  }
}

void
Input::Set(Duk* duk, DukValue container) const
{
  dukglue_push(duk->ctx, container);

  for(auto bind : binds)
  {
    duk_push_number(duk->ctx, bind->value);
    auto rc = duk_put_prop_string(duk->ctx, -2, bind->name.c_str());
    // todo: handle error
  }

  duk_pop(duk->ctx);

  // todo: validate duk stack
}
