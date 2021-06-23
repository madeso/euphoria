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
            core::vec3f const& lhs,
            core::vec3f const& rhs,
            const approx_data& data);

    template <>
    bool
    approximately_equal(
            core::rgb const& lhs,
            core::rgb const& rhs,
            const approx_data& data);

    template <>
    bool
    approximately_equal(
            core::rgba const& lhs,
            core::rgba const& rhs,
            const approx_data& data);

    template <>
    bool
    approximately_equal(
            core::hsl const& lhs,
            core::hsl const& rhs,
            const approx_data& data);

    template <>
    bool
    approximately_equal(
            core::unit3f const& lhs,
            core::unit3f const& rhs,
            const approx_data& data);

    template <>
    bool
    approximately_equal(
            core::quatf const& lhs,
            core::quatf const& rhs,
            const approx_data& data);

    template <>
    bool
    approximately_equal(
            core::axis_angle const& lhs,
            core::axis_angle const& rhs,
            const approx_data& data);

}
