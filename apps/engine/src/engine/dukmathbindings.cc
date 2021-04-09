#include "engine/dukmathbindings.h"

#include <string>
#include <core/sol.h>

#include "core/vec2.h"

#include "core/sol.h"

namespace euphoria::engine
{

template <typename T>
void
BindVec2(Sol* sol, const std::string& name)
{
  using V = core::vec2<T>;

  sol->lua.new_usertype<V>(
      name,
      sol::constructors<V(T, T)>(),
        "x", &V::x,
        "y", &V::y
      );
}

void
BindMath(Sol* duk)
{
  BindVec2<float>(duk, "vec2f");
  // BindVec2<int>(duk, "vec2i");
}

}  // namespace euphoria::engine
