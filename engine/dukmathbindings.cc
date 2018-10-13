#include "engine/dukmathbindings.h"

#include <string>

#include "core/vec2.h"

#include "duk/bindclass.h"
#include "duk/duk.h"


template <typename T>
void
BindVec2(duk::Duk* duk, const std::string& name)
{
  using namespace duk;
  using TStorage = StoreValue<T>;
  using V        = vec2<T, TStorage>;

  duk->BindClass(
      name,
      BindClass<V>()
          .SetConstructor(MakeBind<float, float>(
              [](Context* ctx, float x, float y) -> int {
                return ctx->ReturnObject(std::make_shared<V>(x, y));
              }))
          .AddMethod(
              "getX", MakeBind<V>([](Context* ctx, const V& v) -> int {
                return ctx->ReturnNumber(v.GetX());
              }))
          .AddMethod(
              "getY", MakeBind<V>([](Context* ctx, const V& v) -> int {
                return ctx->ReturnNumber(v.GetY());
              }))
          .AddProperty(
              "x",
              MakeBind<V>([](Context* ctx, const V& v) -> int {
                return ctx->ReturnNumber(v.GetX());
              }),
              MakeBind<V, float>([](Context* ctx, V& v, float x) -> int {
                v.SetX(x);
                return ctx->ReturnVoid();
              }))
          .AddProperty(
              "y",
              MakeBind<V>([](Context* ctx, const V& v) -> int {
                return ctx->ReturnNumber(v.GetY());
              }),
              MakeBind<V, float>([](Context* ctx, V& v, float y) -> int {
                v.SetY(y);
                return ctx->ReturnVoid();
              })));
}

void
BindMath(duk::Duk* duk)
{
  BindVec2<float>(duk, "vec2f");
  // BindVec2<int>(duk, "vec2i");
}
