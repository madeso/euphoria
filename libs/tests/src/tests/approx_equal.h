#pragma once

#include "base/vec3.h"
#include "base/axisangle.h"
#include "base/quat.h"

#include "tests/approx.h"


namespace eu::tests
{
    template <>
    bool
    is_approximately_equal(
            v3 const& lhs,
            v3 const& rhs,
            const ApproxData& data);

    template <>
    bool
    is_approximately_equal(
        v4 const& lhs,
        v4 const& rhs,
        const ApproxData& data);

    template <>
    bool
    is_approximately_equal(
            n3 const& lhs,
            n3 const& rhs,
            const ApproxData& data);

    template <>
    bool
    is_approximately_equal(
            Q const& lhs,
            Q const& rhs,
            const ApproxData& data);

    template <>
    bool
    is_approximately_equal(
            AA const& lhs,
            AA const& rhs,
            const ApproxData& data);

}
