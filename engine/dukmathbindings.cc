#include "engine/dukmathbindings.h"

#include <string>

#include "core/vec2.h"

#include "core/duk.h"


template <typename T>
void
BindVec2(Duk* duk, const std::string& name)
{
  typedef StoreValue<T> TStorage;
  typedef vec2<T, TStorage> V;

  duk->BindClass(
      name,
      BindClass<V>()
          .SetConstructor(Bind{}.bind<float, float>(
              [](Context* ctx, float x, float y) -> int {
                return ctx->ReturnObject(std::make_shared<V>(x, y));
              }))
          .AddMethod(
              "getX", Bind{}.bind<V>([](Context* ctx, const V& v) -> int {
                return ctx->ReturnNumber(v.GetX());
              }))
          .AddMethod(
              "getY", Bind{}.bind<V>([](Context* ctx, const V& v) -> int {
                return ctx->ReturnNumber(v.GetY());
              }))
          .AddProperty(
              "x",
              Bind{}.bind<V>([](Context* ctx, const V& v) -> int {
                return ctx->ReturnNumber(v.GetX());
              }),
              Bind{}.bind<V, float>([](Context* ctx, V& v, float x) -> int {
                v.SetX(x);
                return ctx->ReturnVoid();
              }))
          .AddProperty(
              "y",
              Bind{}.bind<V>([](Context* ctx, const V& v) -> int {
                return ctx->ReturnNumber(v.GetY());
              }),
              Bind{}.bind<V, float>([](Context* ctx, V& v, float y) -> int {
                v.SetY(y);
                return ctx->ReturnVoid();
              })));
}

void
BindMath(Duk* duk)
{
  BindVec2<float>(duk, "vec2f");
  // BindVec2<int>(duk, "vec2i");
}
