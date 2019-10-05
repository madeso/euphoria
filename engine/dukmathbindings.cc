#include "engine/dukmathbindings.h"

#include <string>

#include "core/vec2.h"

#include "duk/bindclass.h"
#include "duk/duk.h"
#include "duk/bind.h"

namespace euphoria::engine
{
  template <typename T>
  void
  BindVec2(duk::Duk* duk, const std::string& name)
  {
    using namespace duk;
    using V = core::vec2<T>;

    duk->BindClass(
        name,
        BindClass<V>()
            .SetConstructor(
                MakeBind<float, float>([](Context* ctx, float x, float y) -> int {
                  return ctx->ReturnObject(std::make_shared<V>(x, y));
                }))
            .AddMethod("getX", MakeBind<V>([](Context* ctx, const V& v) -> int {
                        return ctx->ReturnNumber(v.x);
                      }))
            .AddMethod("getY", MakeBind<V>([](Context* ctx, const V& v) -> int {
                        return ctx->ReturnNumber(v.y);
                      }))
            .AddProperty(
                "x",
                MakeBind<V>([](Context* ctx, const V& v) -> int {
                  return ctx->ReturnNumber(v.x);
                }),
                MakeBind<V, float>([](Context* ctx, V& v, float x) -> int {
                  v.x = x;
                  return ctx->ReturnVoid();
                }))
            .AddProperty(
                "y",
                MakeBind<V>([](Context* ctx, const V& v) -> int {
                  return ctx->ReturnNumber(v.y);
                }),
                MakeBind<V, float>([](Context* ctx, V& v, float y) -> int {
                  v.y = y;
                  return ctx->ReturnVoid();
                })));
  }

  template <typename T>
  void
  BindPoint2(duk::Duk* duk, const std::string& name)
  {
    using namespace duk;
    using V = core::vec2<T>;

    duk->BindClass(
        name,
        BindClass<V>()
            .SetConstructor(
                MakeBind<float, float>([](Context* ctx, float x, float y) -> int {
                  return ctx->ReturnObject(std::make_shared<V>(x, y));
                }))
            .AddMethod("getX", MakeBind<V>([](Context* ctx, const V& v) -> int {
                        return ctx->ReturnNumber(v.x);
                      }))
            .AddMethod("getY", MakeBind<V>([](Context* ctx, const V& v) -> int {
                        return ctx->ReturnNumber(v.y);
                      }))
            .AddProperty(
                "x",
                MakeBind<V>([](Context* ctx, const V& v) -> int {
                  return ctx->ReturnNumber(v.x);
                }),
                MakeBind<V, float>([](Context* ctx, V& v, float x) -> int {
                  v.x = x;
                  return ctx->ReturnVoid();
                }))
            .AddProperty(
                "y",
                MakeBind<V>([](Context* ctx, const V& v) -> int {
                  return ctx->ReturnNumber(v.y);
                }),
                MakeBind<V, float>([](Context* ctx, V& v, float y) -> int {
                  v.y = y;
                  return ctx->ReturnVoid();
                })));
  }

  void
  BindMath(duk::Duk* duk)
  {
    BindVec2<float>(duk, "vec2f");
    BindPoint2<float>(duk, "vec2f");
    // BindVec2<int>(duk, "vec2i");
  }
}
