#include "eu/base/hash.set.h"

namespace eu
{
void HshSt::add(const Hsh& h)
{
    data.emplace(h);
}

HshSt HshSt::combine(const Hsh& h) const
{
    auto clone = data;
    clone.emplace(h);
    return HshSt{.data = clone, .primary = h };
}

bool HshSt::contains(const Hsh& h) const
{
    const auto found = data.find(h);
    return found != data.end();
}

bool HshSt::is(const HshSt& h) const
{
    return contains(h.primary);
}

}
