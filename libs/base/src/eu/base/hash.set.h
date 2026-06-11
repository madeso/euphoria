#pragma once

#include "eu/base/hash.string.h"

#include <set>

namespace eu
{

struct HshSt
{
    std::set<Hsh> data;
    Hsh primary;

    void add(const Hsh& h);
    HshSt combine(const Hsh& h) const;

    bool contains(const Hsh& h) const;
    bool is(const HshSt& h) const;
};

}
