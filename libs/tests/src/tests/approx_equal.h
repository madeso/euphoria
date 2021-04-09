#ifndef TESTS_TESTBASE_H
#define TESTS_TESTBASE_H

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
            const ApproxData&  data);

    template <>
    bool
    approximately_equal(
            core::vec3f const& lhs,
            core::vec3f const& rhs,
            const ApproxData&  data);

    template <>
    bool
    approximately_equal(
            core::Rgb const&  lhs,
            core::Rgb const&  rhs,
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
            core::Hsl const&  lhs,
            core::Hsl const&  rhs,
            const ApproxData& data);

    template <>
    bool
    approximately_equal(
            core::unit3f const& lhs,
            core::unit3f const& rhs,
            const ApproxData&   data);

    template <>
    bool
    approximately_equal(
            core::quatf const& lhs,
            core::quatf const& rhs,
            const ApproxData&  data);

    template <>
    bool
    approximately_equal(
            core::AxisAngle const& lhs,
            core::AxisAngle const& rhs,
            const ApproxData&      data);

}  // namespace euphoria::tests

#endif  // TESTS_TESTBASE_H
