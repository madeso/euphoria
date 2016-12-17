#ifndef TESTS_TESTBASE_H
#define TESTS_TESTBASE_H

#include "gtest/gtest.h"

#include "core/vec3.h"

::testing::AssertionResult almost_equal(const char* lhs_expression, const char* rhs_expression, const vec3f& lhs_value, const vec3f& rhs_value);

#endif  // TESTS_TESTBASE_H