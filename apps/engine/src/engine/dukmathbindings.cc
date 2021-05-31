#include "engine/dukmathbindings.h"

#include <string>
#include <core/sol.h>

#include "core/vec2.h"

#include "core/sol.h"

namespace euphoria::engine
{

    template <typename T>
    void
    bind_vec2(Sol* sol, const std::string& name)
    {
        using V = core::vec2<T>;

        sol->lua.new_usertype<V>
        (
            name,
            sol::constructors<V(T, T)>(),
            "x", &V::x,
            "y", &V::y
        );
    }

    void
    bind_math(Sol* duk)
    {
        bind_vec2<float>(duk, "vec2f");
        // bind_vec2<int>(duk, "vec2i");
    }

}
