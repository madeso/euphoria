#include "eu/base/axisangle.h"

#include <algorithm>

#include "eu/assert/assert.h"

#include "eu/base/quat.h"

namespace eu
{
    AA::AA(const n3 &ax, const An &ang)
        : axis(ax), angle(ang)
    {
        ASSERT(ax.is_valid());
    }

    [[nodiscard]] AA
    AA::from(const Q& q)
    {
        const float cos_a = std::ranges::clamp(q.w , -1.0f, 1.0f); // todo(Gustav): is this needed?
        const auto angle = acos(cos_a) * 2;
        const auto sin_a = clamp_zero(std::sqrt(1.0f - cos_a * cos_a), 1, 0.0005f);
        // todo(Gustav): do we need to normalize here?
        const auto axis = (q.get_vec_part() / sin_a).get_normalized();
        if (!axis)
        {
            // a "zero" rotation so any axis works
            return rha(kk::up, no_rotation);
        }
        return rha(*axis, angle);
    }

    [[nodiscard]] Ypr
    Ypr::from(const Q& q)
    {
        // Protect against small floating-point errors.
        const auto sin_pitch = std::clamp(
            2.0f * (q.y * q.z - q.w * q.x),
            -1.0f,
            1.0f
        );

        return
        {
            .yaw = An::from_radians(std::atan2(
                -2.0f * (q.w * q.y + q.x * q.z),
                1.0f - 2.0f * (q.x * q.x + q.y * q.y)
            )),
            .pitch = An::from_radians(std::asin(sin_pitch)),
            .roll = An::from_radians(std::atan2(
                2.0f * (q.w * q.z + q.x * q.y),
                1.0f - 2.0f * (q.x * q.x + q.z * q.z)
            ))
        };
    }

    AA
    rha(const n3 &axis, const An &angle)
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
