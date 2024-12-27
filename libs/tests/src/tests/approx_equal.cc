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

    template <typename T>
    bool
    is_approximately_equal_xy(T const& lhs, T const& rhs, const ApproxData& data)
    {
        return is_approximately_equal(lhs.x, rhs.x, data)
               && is_approximately_equal(lhs.y, rhs.y, data);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <>
    bool
    is_approximately_equal(
            v2 const& lhs,
            v2 const& rhs,
            const ApproxData& data)
    {
        return is_approximately_equal_xy(lhs, rhs, data);
    }

    template <>
    bool
    is_approximately_equal(
            n2 const& lhs,
            n2 const& rhs,
            const ApproxData& data)
    {
        return is_approximately_equal_xy(lhs, rhs, data);
    }

    template <>
    bool
    is_approximately_equal(
            v3 const& lhs,
            v3 const& rhs,
            const ApproxData& data)
    {
        return is_approximately_equal_xyz(lhs, rhs, data);
    }

    template <>
    bool
    is_approximately_equal(
            v4 const& lhs,
            v4 const& rhs,
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
            n3 const& lhs,
            n3 const& rhs,
            const ApproxData& data)
    {
        return is_approximately_equal_xyz(lhs, rhs, data);
    }


    template <>
    bool
    is_approximately_equal(
            Q const& lhs,
            Q const& rhs,
            const ApproxData& data)
    {
        return is_approximately_equal(1.0f, dot(lhs, rhs), data);
    }

    template <>
    bool
    is_approximately_equal(
            AA const& lhs,
            AA const& rhs,
            const ApproxData& data)
    {
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
    is_approximately_equal(m4 const& lhs, m4 const& rhs, const ApproxData& data)
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
