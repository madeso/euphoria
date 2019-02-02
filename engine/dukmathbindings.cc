#include "engine/dukmathbindings.h"

#include <string>
#include <core/sol.h>

#include "core/vec2.h"

#include "core/sol.h"

template <typename T>
void
BindVec2(sol::state* sol, const std::string& name)
{
  using V = vec2<T>;

  sol->new_usertype<V>(
      name,
      sol::constructors<V(T, T)>(),
        "x", &V::x,
        "y", &V::y
      );
}

template <typename T>
void
BindPoint2(sol::state* sol, const std::string& name)
{
  using V = point2<T>;

  sol->new_usertype<V>(
      name,
      sol::constructors<V(T, T)>(),
      "x", &V::x,
      "y", &V::y
  );
}

void
BindMath(sol::state* duk)
{
  BindVec2<float>(duk, "vec2f");
  BindPoint2<float>(duk, "point2f");
  // BindVec2<int>(duk, "vec2i");
}
