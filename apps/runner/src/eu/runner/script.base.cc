#include "eu/runner/script.base.h"

#include "eu/runner/script.h"

namespace
{
template<typename T, typename F>
void bind_binary(lax::ClassAdder<T>* k, lax::Lax* lax, const char* const name, F op)
{
    k->add_static_function(name, [lax, op](lax::Callable*, lax::ArgumentHelper& arguments)->std::shared_ptr<lax::Object>
    {
        auto lhs = arguments.require_native<T>("lhs");
        auto rhs = arguments.require_native<T>("rhs");
        if (arguments.complete())
        {
            return lax::make_nil();
        }
        return lax->make_native<T>(op(*lhs, *rhs));
    });
}
}

namespace eu::runner
{
// todo(Gustav): add option to switch lax from double to float, or just switch to float and stop caring about doubles
float fl(lax::Tf d) { return static_cast<float>(d); }
lax::Tf lf(float f) { return static_cast<lax::Tf>(f); }

void register_vec3(lax::Lax* lax, lax::Scope* pkg)
{
    auto k = pkg->define_native_class<v3>("v3", [](auto& args)
    {
        const auto x = fl(args.require_float("x"));
        const auto y = fl(args.require_float("y"));
        const auto z = fl(args.require_float("z"));
        if (args.complete())
        {
            return v3{ 0,0,0 };
        }
        return v3{ x, y, z };
    });
    k.add_property<lax::Tf>("x",
        [](const v3& v) -> lax::Tf{ return lf(v.x); },
        [](auto& v, lax::Tf value) { v.x = fl(value); }
    );
    k.add_property<lax::Tf>("y",
        [](const v3& v) -> lax::Tf{ return lf(v.y); },
        [](auto& v, lax::Tf value) { v.y = fl(value); }
    );
    k.add_property<lax::Tf>("z",
        [](const v3& v) -> lax::Tf{ return lf(v.z); },
        [](auto& v, lax::Tf value) { v.z = fl(value); }
    );
    bind_binary(&k, lax, lax::named::functions::add, [](const v3& lhs, const v3& rhs) { return lhs + rhs; });
    bind_binary(&k, lax, lax::named::functions::sub, [](const v3& lhs, const v3& rhs) { return lhs - rhs; });

    pkg->add_native_getter("up", [lax] {return lax->make_native(kk::up);});
    pkg->add_native_getter("down", [lax] {return lax->make_native(kk::down); });
    pkg->add_native_getter("left", [lax] {return lax->make_native(kk::left); });
    pkg->add_native_getter("right", [lax] {return lax->make_native(kk::right); });
    pkg->add_native_getter("in", [lax] {return lax->make_native(kk::in); });
    pkg->add_native_getter("out", [lax] {return lax->make_native(kk::out); });
}

void register_base(Script* script)
{
    auto pkg = script->lax.in_package("eu");
    register_vec3(&script->lax, pkg.get());
}

}
