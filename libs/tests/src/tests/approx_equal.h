#pragma once

#include "core/vec3.h"
#include "core/axisangle.h"
#include "core/quat.h"
#include "core/rgb.h"

#include "tests/approx.h"


namespace euphoria::tests
{
    template <>
    bool
    is_approximately_equal(
            core::vec3f const& lhs,
            core::vec3f const& rhs,
            const ApproxData& data);

    template <>
    bool
    is_approximately_equal(
        core::vec4f const& lhs,
        core::vec4f const& rhs,
        const ApproxData& data);

    template <>
    bool
    is_approximately_equal(
            core::rgb const& lhs,
            core::rgb const& rhs,
            const ApproxData& data);

    template <>
    bool
    is_approximately_equal(
            core::rgba const& lhs,
            core::rgba const& rhs,
            const ApproxData& data);

    template <>
    bool
    is_approximately_equal(
            core::Hsl const& lhs,
            core::Hsl const& rhs,
            const ApproxData& data);

    template <>
    bool
    is_approximately_equal(
            core::unit3f const& lhs,
            core::unit3f const& rhs,
            const ApproxData& data);

    template <>
    bool
    is_approximately_equal(
            core::quatf const& lhs,
            core::quatf const& rhs,
            const ApproxData& data);

    template <>
    bool
    is_approximately_equal(
            core::AxisAngle const& lhs,
            core::AxisAngle const& rhs,
            const ApproxData& data);

}
