#include "tests/testbase.h"

template <>
bool
approximately_equal(vec3f const& lhs, vec3f const& rhs, const ApproxData& data)
{
  return approximately_equal(lhs.x, rhs.x, data) &&
         approximately_equal(lhs.y, rhs.y, data) &&
         approximately_equal(lhs.z, rhs.z, data);
}

template <>
bool
approximately_equal(Rgba const& lhs, Rgba const& rhs, const ApproxData& data)
{
  return approximately_equal(lhs.GetRed(), rhs.GetRed(), data) &&
         approximately_equal(lhs.GetGreen(), rhs.GetGreen(), data) &&
         approximately_equal(lhs.GetBlue(), rhs.GetBlue(), data) &&
         approximately_equal(lhs.GetAlpha(), rhs.GetAlpha(), data);
}

template <>
bool
approximately_equal(
    vec3f::Unit const& lhs, vec3f::Unit const& rhs, const ApproxData& data)
{
  return approximately_equal(lhs.x, rhs.x, data) &&
         approximately_equal(lhs.y, rhs.y, data) &&
         approximately_equal(lhs.z, rhs.z, data);
}

template <>
bool
approximately_equal(quatf const& lhs, quatf const& rhs, const ApproxData& data)
{
  return approximately_equal(1.0f, dot(lhs, rhs), data);
}

template <>
bool
approximately_equal(
    AxisAngle const& lhs, AxisAngle const& rhs, const ApproxData& data)
{
  if(approximately_equal(lhs.angle.InDegrees(), rhs.angle.InDegrees(), data) &&
     approximately_equal(lhs.angle.InDegrees(), 0.0f, data))
  {
    return true;  // zero rotation is always equal zero
  }

  const bool a =
      (approximately_equal(rhs.axis, lhs.axis, data) &&
       approximately_equal(rhs.angle.InDegrees(), lhs.angle.InDegrees(), data));
  const bool inv =
      (approximately_equal(rhs.axis, -lhs.axis, data) &&
       approximately_equal(
           rhs.angle.InDegrees(), -lhs.angle.InDegrees(), data));
  return a || inv;
}
