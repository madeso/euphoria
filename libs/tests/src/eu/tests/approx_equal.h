#pragma once

#include "eu/base/vec2.h"
#include "eu/base/vec3.h"
#include "eu/base/axisangle.h"
#include "eu/base/quat.h"

#include "eu/tests/approx.h"


namespace eu::tests
{
    template <>
    bool
    is_approximately_equal(
            v2 const& lhs,
            v2 const& rhs,
            const ApproxData& data);

    template <>
    bool
    is_approximately_equal(
            n2 const& lhs,
            n2 const& rhs,
            const ApproxData& data);

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
