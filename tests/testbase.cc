#include "tests/testbase.h"

#include "core/numeric.h"

bool IsEqual(const vec3f& lhs_value, const vec3f& rhs_value) {
  return IsEqual(lhs_value.x, rhs_value.x)
        && ::IsEqual(lhs_value.y, rhs_value.y)
        && ::IsEqual(lhs_value.z, rhs_value.z)
  ;
}

bool IsEqual(const quatf& lhs_value, const quatf& rhs_value) {
  return IsEqual(1.0f, dot(lhs_value, rhs_value));
  /*
  return IsEqual(lhs_value.x, rhs_value.x)
         && ::IsEqual(lhs_value.y, rhs_value.y)
         && ::IsEqual(lhs_value.z, rhs_value.z)
         && ::IsEqual(lhs_value.w, rhs_value.w)
      ;
  */
}

template<typename T>
::testing::AssertionResult almost_equal_base(const char* lhs_expression, const char* rhs_expression, const T& lhs_value, const T& rhs_value) {
  const bool almost_equal = ::IsEqual(lhs_value, rhs_value);
  if( almost_equal) return ::testing::AssertionSuccess();

  ::std::stringstream lhs_ss;
  lhs_ss << std::setprecision(std::numeric_limits<float>::digits10 + 2)
         << lhs_value;

  ::std::stringstream rhs_ss;
  rhs_ss << std::setprecision(std::numeric_limits<float>::digits10 + 2)
         << rhs_value;

  return ::testing::internal::EqFailure(lhs_expression,
                                        rhs_expression,
                                        ::testing::internal::StringStreamToString(&lhs_ss),
                                        ::testing::internal::StringStreamToString(&rhs_ss),
                                        false);
}

::testing::AssertionResult almost_equal(const char* lhs_expression, const char* rhs_expression, const vec3f& lhs_value, const vec3f& rhs_value) {
  return almost_equal_base(lhs_expression, rhs_expression, lhs_value, rhs_value);
}

::testing::AssertionResult almost_equal_quat(const char* lhs_expression, const char* rhs_expression, const quatf& lhs_value, const quatf& rhs_value) {
  return almost_equal_base(lhs_expression, rhs_expression, lhs_value, rhs_value);
}