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
    approximately_equal(
            core::Vec3f const& lhs,
            core::Vec3f const& rhs,
            const ApproxData& data);

    template <>
    bool
    approximately_equal(
            core::Rgb const& lhs,
            core::Rgb const& rhs,
            const ApproxData& data);

    template <>
    bool
    approximately_equal(
            core::Rgba const& lhs,
            core::Rgba const& rhs,
            const ApproxData& data);

    template <>
    bool
    approximately_equal(
            core::Hsl const& lhs,
            core::Hsl const& rhs,
            const ApproxData& data);

    template <>
    bool
    approximately_equal(
            core::Unit3f const& lhs,
            core::Unit3f const& rhs,
            const ApproxData& data);

    template <>
    bool
    approximately_equal(
            core::Quatf const& lhs,
            core::Quatf const& rhs,
            const ApproxData& data);

    template <>
    bool
    approximately_equal(
            core::AxisAngle const& lhs,
            core::AxisAngle const& rhs,
            const ApproxData& data);

}
