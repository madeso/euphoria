#include "base/axisangle.h"

#include "assert/assert.h"

namespace eu
{
    AA::AA(const n3 &ax, const An &ang)
        : axis(ax), angle(ang)
    {
        ASSERT(ax.is_valid());
    }

    AA
    right_hand_around(const n3 &axis, const An &angle)
    {
        ASSERT(axis.is_valid());
        return {axis, An::from_radians(angle.as_radians())};
    }

    std::string
    string_from(const AA &aa)
    {
        return fmt::format("({} {})", aa.axis, aa.angle);
    }

    std::string
    string_from(const Ypr& x)
    {
        return fmt::format("({} {} {})", x.yaw, x.pitch, x.roll);
    }
}
