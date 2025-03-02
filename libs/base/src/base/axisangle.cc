#include "base/axisangle.h"

#include "assert/assert.h"

#include "base/quat.h"

namespace eu
{
    AA::AA(const n3 &ax, const An &ang)
        : axis(ax), angle(ang)
    {
        ASSERT(ax.is_valid());
    }

    [[nodiscard]] std::optional<AA>
    AA::from(const Q& q)
    {
        const float cos_a = q.w;
        const auto angle = acos(cos_a) * 2;
        const auto sin_a = get_default_if_close_to_zero<float>(
                sqrt(1.0f - cos_a * cos_a), 1, 0.0005f);
        // todo(Gustav): do we need to normalize here?
        const auto axis = (q.get_vec_part() / sin_a).get_normalized();
        if (!axis)
            { return std::nullopt; }
        return right_hand_around(*axis, angle);
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
        return fmt::format("({}, {}, {})", x.yaw, x.pitch, x.roll);
    }

    ADD_CATCH_FORMATTER_IMPL(AA)
    ADD_CATCH_FORMATTER_IMPL(Ypr)
}
