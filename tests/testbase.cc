#include "tests/testbase.h"

#include "core/numeric.h"

::testing::AssertionResult almost_equal(const char* lhs_expression, const char* rhs_expression, const vec3f& lhs_value, const vec3f& rhs_value) {
  const bool almost_equal = ::IsEqual(lhs_value.x, rhs_value.x)
                            && ::IsEqual(lhs_value.y, rhs_value.y)
                            && ::IsEqual(lhs_value.z, rhs_value.z);
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
