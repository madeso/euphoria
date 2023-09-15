#include "tests/approx_equal.h"

namespace eu::tests
{
    template <typename T>
    bool
    is_approximately_equal_xyz(T const& lhs, T const& rhs, const ApproxData& data)
    {
        return is_approximately_equal(lhs.x, rhs.x, data)
               && is_approximately_equal(lhs.y, rhs.y, data)
               && is_approximately_equal(lhs.z, rhs.z, data);
    }

    template <>
    bool
    is_approximately_equal(
            core::vec3f const& lhs,
            core::vec3f const& rhs,
            const ApproxData& data)
    {
        return is_approximately_equal_xyz(lhs, rhs, data);
    }

    template <>
    bool
    is_approximately_equal(
            core::vec4f const& lhs,
            core::vec4f const& rhs,
            const ApproxData& data)
    {
        return is_approximately_equal(lhs.x, rhs.x, data)
               && is_approximately_equal(lhs.y, rhs.y, data)
               && is_approximately_equal(lhs.z, rhs.z, data)
               && is_approximately_equal(lhs.w, rhs.w, data)
            ;
    }

    template <>
    bool
    is_approximately_equal(
            core::Rgb const& lhs,
            core::Rgb const& rhs,
            const ApproxData& data)
    {
        return is_approximately_equal(lhs.r, rhs.r, data)
               && is_approximately_equal(lhs.g, rhs.g, data)
               && is_approximately_equal(lhs.b, rhs.b, data);
    }

    template <>
    bool
    is_approximately_equal(
            core::Hsl const& lhs,
            core::Hsl const& rhs,
            const ApproxData& data)
    {
        return is_approximately_equal(lhs.h.as_degrees(), rhs.h.as_degrees(), data)
               && is_approximately_equal(lhs.s * 100, rhs.s * 100, data)
               && is_approximately_equal(lhs.l * 100, rhs.l * 100, data);
    }

    template <>
    bool
    is_approximately_equal(
            core::unit3f const& lhs,
            core::unit3f const& rhs,
            const ApproxData& data)
    {
        return is_approximately_equal_xyz(lhs, rhs, data);
    }

    template <>
    bool
    is_approximately_equal(
            core::Rgba const& lhs,
            core::Rgba const& rhs,
            const ApproxData& data)
    {
        return is_approximately_equal(lhs.r, rhs.r, data)
               && is_approximately_equal(lhs.g, rhs.g, data)
               && is_approximately_equal(lhs.b, rhs.b, data)
               && is_approximately_equal(lhs.a, rhs.a, data);
    }


    template <>
    bool
    is_approximately_equal(
            core::quatf const& lhs,
            core::quatf const& rhs,
            const ApproxData& data)
    {
        return is_approximately_equal(1.0f, dot(lhs, rhs), data);
    }

    template <>
    bool
    is_approximately_equal(
            core::AxisAngle const& lhs,
            core::AxisAngle const& rhs,
            const ApproxData& data)
    {
        if(is_approximately_equal(
                   lhs.angle.as_degrees(), rhs.angle.as_degrees(), data)
           && is_approximately_equal(lhs.angle.as_degrees(), 0.0f, data))
        {
            return true; // zero rotation is always equal zero
        }

        const bool a =
        (
            is_approximately_equal(rhs.axis, lhs.axis, data) &&
            is_approximately_equal(rhs.angle.as_degrees(), lhs.angle.as_degrees(), data)
        );
        const bool inv =
        (
            is_approximately_equal(rhs.axis, -lhs.axis, data) &&
            is_approximately_equal(rhs.angle.as_degrees(), -lhs.angle.as_degrees(), data)
        );
        return a || inv;
    }


    template <>
    bool
    is_approximately_equal(core::mat4f const& lhs, core::mat4f const& rhs, const ApproxData& data)
    {
        for (int row_index = 0; row_index < 4; row_index += 1)
        {
            for (int column_index = 0; column_index < 4; column_index += 1)
            {
                if (is_approximately_equal(lhs.get(row_index,column_index), rhs.get(row_index, column_index), data) == false)
                {
                    return false;
                }
            }
        }
        return true;
    }

}
