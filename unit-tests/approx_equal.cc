#include "approx_equal.h"

namespace euphoria::tests
{
    template <typename T>
    bool
    vec3_approximately_equal(T const& lhs, T const& rhs, const ApproxData& data)
    {
        return approximately_equal(lhs.x, rhs.x, data)
               && approximately_equal(lhs.y, rhs.y, data)
               && approximately_equal(lhs.z, rhs.z, data);
    }

    template <>
    bool
    approximately_equal(
            core::vec3f const& lhs,
            core::vec3f const& rhs,
            const ApproxData&  data)
    {
        return vec3_approximately_equal(lhs, rhs, data);
    }

    template <>
    bool
    approximately_equal(
            core::Rgb const&  lhs,
            core::Rgb const&  rhs,
            const ApproxData& data)
    {
        return approximately_equal(lhs.r, rhs.r, data)
               && approximately_equal(lhs.g, rhs.g, data)
               && approximately_equal(lhs.b, rhs.b, data);
    }

    template <>
    bool
    approximately_equal(
            core::Hsl const&  lhs,
            core::Hsl const&  rhs,
            const ApproxData& data)
    {
        return approximately_equal(lhs.h.InDegrees(), rhs.h.InDegrees(), data)
               && approximately_equal(lhs.s * 100, rhs.s * 100, data)
               && approximately_equal(lhs.l * 100, rhs.l * 100, data);
    }

    template <>
    bool
    approximately_equal(
            core::unit3f const& lhs,
            core::unit3f const& rhs,
            const ApproxData&   data)
    {
        return vec3_approximately_equal(lhs, rhs, data);
    }

    template <>
    bool
    approximately_equal(
            core::Rgba const& lhs,
            core::Rgba const& rhs,
            const ApproxData& data)
    {
        return approximately_equal(lhs.r, rhs.r, data)
               && approximately_equal(lhs.g, rhs.g, data)
               && approximately_equal(lhs.b, rhs.b, data)
               && approximately_equal(lhs.a, rhs.a, data);
    }


    template <>
    bool
    approximately_equal(
            core::quatf const& lhs,
            core::quatf const& rhs,
            const ApproxData&  data)
    {
        return approximately_equal(1.0f, dot(lhs, rhs), data);
    }

    template <>
    bool
    approximately_equal(
            core::AxisAngle const& lhs,
            core::AxisAngle const& rhs,
            const ApproxData&      data)
    {
        if(approximately_equal(
                   lhs.angle.InDegrees(), rhs.angle.InDegrees(), data)
           && approximately_equal(lhs.angle.InDegrees(), 0.0f, data))
        {
            return true;  // zero rotation is always equal zero
        }

        const bool a
                = (approximately_equal(rhs.axis, lhs.axis, data)
                   && approximately_equal(
                           rhs.angle.InDegrees(), lhs.angle.InDegrees(), data));
        const bool inv
                = (approximately_equal(rhs.axis, -lhs.axis, data)
                   && approximately_equal(
                           rhs.angle.InDegrees(),
                           -lhs.angle.InDegrees(),
                           data));
        return a || inv;
    }

}  // namespace euphoria::tests