#include "runner/dukmathbindings.h"

#include <string>

#include "core/cint.h"
#include "core/vec2.h"
#include "core/sol.h"

namespace euphoria::runner
{

    template <typename T>
    void
    bind_vec2(LuaState* sol, const std::string& name)
    {
        using V = core::Vec2<T>;

        sol::usertype<V> t = sol->state.new_usertype<V>
        (
            name,
            sol::constructors<V(T, T)>()
        );

        t["x"] = sol::property
        (
            [](const V& v) -> double
            {
                return core::c_float_to_double(v.x);
            },
            [](V& v, double x)
            {
                v.x = core::c_double_to_float(x);
            }
        );

        t["y"] = sol::property
        (
            [](const V& v) -> double
            {
                return core::c_float_to_double(v.y);
            },
            [](V& v, double y)
            {
                v.y = core::c_double_to_float(y);
            }
        );
    }

    void
    bind_math(LuaState* duk)
    {
        bind_vec2<float>(duk, "vec2f");
        // bind_vec2<int>(duk, "vec2i");
    }

}
