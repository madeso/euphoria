#include "engine/dukmathbindings.h"

#include <string>

#include "core/vec2.h"

#include "engine/duk.h"

#include "dukglue/dukglue.h"

template <typename T, typename TStorage>
void
BindVec2(Duk* duk, const std::string& name)
{
  dukglue_register_property(
      duk->ctx, &vec2<T, TStorage>::GetX, &vec2<T, TStorage>::SetX, "x");
  dukglue_register_property(
      duk->ctx, &vec2<T, TStorage>::GetY, &vec2<T, TStorage>::SetY, "y");

  dukglue_register_constructor<vec2<T, TStorage>, T, T>(duk->ctx, name.c_str());
  dukglue_register_method(duk->ctx, &vec2<T, TStorage>::GetY, "getY");
  dukglue_register_method(duk->ctx, &vec2<T, TStorage>::SetY, "setY");
}

void
BindMath(Duk* duk)
{
  BindVec2<float, StoreValue<float>>(duk, "vec2f");
  // BindVec2<float, StorePointer<float>>(duk, "vec2fp");
  // BindVec2<int>(duk, "vec2i");
}
