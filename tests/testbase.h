#ifndef TESTS_TESTBASE_H
#define TESTS_TESTBASE_H

#include "gtest/gtest.h"

#include "core/vec3.h"
#include "core/quat.h"
#include "core/axisangle.h"

::testing::AssertionResult almost_equal(const char* lhs_expression, const char* rhs_expression, const vec3f& lhs_value, const vec3f& rhs_value);
::testing::AssertionResult almost_equal_quat(const char* lhs_expression, const char* rhs_expression, const quatf& lhs_value, const quatf& rhs_value);
::testing::AssertionResult almost_equal_axisangle(const char* lhs_expression, const char* rhs_expression, const AxisAngle& lhs_value, const AxisAngle& rhs_value);

#endif  // TESTS_TESTBASE_H