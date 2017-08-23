#include "tests/approx.h"

template <>
bool
approximately_equal(double const& lhs, double const& rhs,
                    const ApproxData& data)
{
  // Thanks to Richard Harris for his help refining this formula
  auto lhs_v      = lhs;
  bool relativeOK = std::fabs(lhs_v - rhs) <
                    data.epsilon * (data.scale + (std::max)(std::fabs(lhs_v),
                                                            std::fabs(rhs)));
  if(relativeOK)
  {
    return true;
  }
  return std::fabs(lhs_v - rhs) < data.margin;
}

template <>
bool
approximately_equal(float const& lhs, float const& rhs, const ApproxData& data)
{
  return approximately_equal(static_cast<double>(lhs), static_cast<double>(rhs),
                             data);
}
