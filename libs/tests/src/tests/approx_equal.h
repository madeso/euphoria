#pragma once

#include "base/vec3.h"
#include "base/axisangle.h"
#include "base/quat.h"
#include "core/rgb.h"

#include "tests/approx.h"


namespace eu::tests
{
    template <>
    bool
    is_approximately_equal(
            vec3f const& lhs,
            vec3f const& rhs,
            const ApproxData& data);

    template <>
    bool
    is_approximately_equal(
        vec4f const& lhs,
        vec4f const& rhs,
        const ApproxData& data);

    template <>
    bool
    is_approximately_equal(
            core::Rgb const& lhs,
            core::Rgb const& rhs,
            const ApproxData& data);

    template <>
    bool
    is_approximately_equal(
            core::Rgba const& lhs,
            core::Rgba const& rhs,
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
            unit3f const& lhs,
            unit3f const& rhs,
            const ApproxData& data);

    template <>
    bool
    is_approximately_equal(
            quatf const& lhs,
            quatf const& rhs,
            const ApproxData& data);

    template <>
    bool
    is_approximately_equal(
            AxisAngle const& lhs,
            AxisAngle const& rhs,
            const ApproxData& data);

}
