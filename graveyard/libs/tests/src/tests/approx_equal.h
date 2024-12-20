#pragma once

#include "base/vec3.h"
#include "base/axisangle.h"
#include "base/quat.h"
#include "base/rgb.h"

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
            Rgb const& lhs,
            Rgb const& rhs,
            const ApproxData& data);

    template <>
    bool
    is_approximately_equal(
            Rgba const& lhs,
            Rgba const& rhs,
            const ApproxData& data);

    template <>
    bool
    is_approximately_equal(
            Hsl const& lhs,
            Hsl const& rhs,
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
