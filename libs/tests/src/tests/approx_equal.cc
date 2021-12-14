#include "tests/approx_equal.h"

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
            core::Vec3f const& lhs,
            core::Vec3f const& rhs,
            const ApproxData& data)
    {
        return vec3_approximately_equal(lhs, rhs, data);
    }

    template <>
    bool
    approximately_equal(
            core::Rgb const& lhs,
            core::Rgb const& rhs,
            const ApproxData& data)
    {
        return approximately_equal(lhs.r, rhs.r, data)
               && approximately_equal(lhs.g, rhs.g, data)
               && approximately_equal(lhs.b, rhs.b, data);
    }

    template <>
    bool
    approximately_equal(
            core::Hsl const& lhs,
            core::Hsl const& rhs,
            const ApproxData& data)
    {
        return approximately_equal(lhs.h.in_degrees(), rhs.h.in_degrees(), data)
               && approximately_equal(lhs.s * 100, rhs.s * 100, data)
               && approximately_equal(lhs.l * 100, rhs.l * 100, data);
    }

    template <>
    bool
    approximately_equal(
            core::Unit3f const& lhs,
            core::Unit3f const& rhs,
            const ApproxData& data)
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
            core::Quatf const& lhs,
            core::Quatf const& rhs,
            const ApproxData& data)
    {
        return approximately_equal(1.0f, dot(lhs, rhs), data);
    }

    template <>
    bool
    approximately_equal(
            core::AxisAngle const& lhs,
            core::AxisAngle const& rhs,
            const ApproxData& data)
    {
        if(approximately_equal(
                   lhs.angle.in_degrees(), rhs.angle.in_degrees(), data)
           && approximately_equal(lhs.angle.in_degrees(), 0.0f, data))
        {
            return true; // zero rotation is always equal zero
        }

        const bool a
                = (approximately_equal(rhs.axis, lhs.axis, data)
                   && approximately_equal(
                           rhs.angle.in_degrees(), lhs.angle.in_degrees(), data));
        const bool inv
                = (approximately_equal(rhs.axis, -lhs.axis, data)
                   && approximately_equal(
                           rhs.angle.in_degrees(),
                           -lhs.angle.in_degrees(),
                           data));
        return a || inv;
    }

}
