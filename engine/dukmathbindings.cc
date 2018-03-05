#include "engine/dukmathbindings.h"

#include <string>

#include "core/vec2.h"

#include "engine/duk.h"

#include "dukglue/dukglue.h"

template <typename T>
void
BindVec2(Duk* duk, const std::string& name)
{
  dukglue_register_property(duk->ctx, &vec2<T>::GetX, &vec2<T>::SetX, "x");
  dukglue_register_property(duk->ctx, &vec2<T>::GetY, &vec2<T>::SetY, "y");

  dukglue_register_constructor<vec2<T>, T, T>(duk->ctx, name.c_str());
  dukglue_register_method(duk->ctx, &vec2<T>::GetY, "getY");
  dukglue_register_method(duk->ctx, &vec2<T>::SetY, "setY");
}

void
BindMath(Duk* duk)
{
  BindVec2<float>(duk, "vec2f");
  // BindVec2<int>(duk, "vec2i");
}
