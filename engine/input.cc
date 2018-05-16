#include "engine/input.h"

#include "core/assert.h"
#include "core/duk.h"

BoundVar::BoundVar(const std::string& n, const Key& k)
    : name(n)
    , state(0)
    , last_state(0)
    , key(k)
{
}

void
BindBoundVar(Duk* duk)
{
  duk->BindClass(
      "BoundVar",
      BindClass<BoundVar>()
          .AddProperty(
              "state",
              Bind{}.bind<BoundVar>(
                  [](Context* ctx, const BoundVar& bv) -> int {
                    return ctx->ReturnNumber(bv.state);
                  }),
              Bind{})
          .AddProperty(
              "last_state",
              Bind{}.bind<BoundVar>(
                  [](Context* ctx, const BoundVar& bv) -> int {
                    return ctx->ReturnNumber(bv.last_state);
                  }),
              Bind{}));
}

void
Input::Bind(Duk* duk)
{
  BindBoundVar(duk);
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
  for(const auto& bind : binds)
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
  for(const auto& bind : binds)
  {
    container.SetFree(duk, bind->name, bind.get());
  }
// todo: figure out how to best do this? class bind? need to work with TS
#if 0
  dukglue_push(duk->ctx, container);

  duk_bool_t rc;

  for(const auto& bind : binds)
  {
    duk_push_object(duk->ctx);                        // object
    duk_push_number(duk->ctx, bind->state);           // object state
    rc = duk_put_prop_string(duk->ctx, -2, "state");  // object
    if(rc != 1)
    {
      DIE("Failed to set state");
    }
    duk_push_number(duk->ctx, bind->last_state);           // object last_state
    rc = duk_put_prop_string(duk->ctx, -2, "last_state");  // object
    if(rc != 1)
    {
      DIE("Failed to set last state");
    }
    rc = duk_put_prop_string(duk->ctx, -2, bind->name.c_str());  //
    if(rc != 1)
    {
      DIE("Failed to set key");
    }
    // todo: handle error
  }

  duk_pop(duk->ctx);

  // todo: validate duk stack
#endif
}

void
Input::UpdateState()
{
  for(auto& bind : binds)
  {
    bind->last_state = bind->state;
  }
}
