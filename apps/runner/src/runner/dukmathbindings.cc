#include "runner/dukmathbindings.h"



#include "base/cint.h"
#include "base/vec2.h"
#include "core/sol.h"

namespace eu::runner
{

    void
    bind_vec2(LuaState* sol, const std::string& name)
    {
        using V = vec2f;

        sol::usertype<V> t = sol->state.new_usertype<V>
        (
            name,
            sol::constructors<V(float, float)>()
        );

        t["x"] = sol::property
        (
            [](const V& v) -> double
            {
                return c_float_to_double(v.x);
            },
            [](V& v, double x)
            {
                v.x = c_double_to_float(x);
            }
        );

        t["y"] = sol::property
        (
            [](const V& v) -> double
            {
                return c_float_to_double(v.y);
            },
            [](V& v, double y)
            {
                v.y = c_double_to_float(y);
            }
        );
    }

    void
    bind_math(LuaState* duk)
    {
        bind_vec2(duk, "vec2f");
        // bind_vec2<int>(duk, "vec2i");
    }

}
