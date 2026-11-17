#pragma once

namespace eu
{

template<typename T>
struct Defer
{
    T defer;
    explicit Defer(T t) : defer(t) {}
    ~Defer()
    {
        defer();
    }

    explicit Defer(const Defer&) = delete;
    explicit Defer(Defer&&) = delete;
    void operator=(const Defer&) = delete;
    void operator=(Defer&&) = delete;
};

}

#define EU_JOIN(lhs, rhs) lhs ## rhs
#define EU_PASTE(lhs, rhs) EU_JOIN(lhs, rhs)
#define EU_DEFER(x) const auto EU_PASTE(defer_, __LINE__) = ::eu::Defer{[&] {x;}}
