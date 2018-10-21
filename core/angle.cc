#include "core/angle.h"

#include "core/assert.h"
#include "core/range.h"

#include <cmath>

void
Angle::Wrap()
{
  mRad = Range{0.0f, Pi() * 2.0f}.Wrap(mRad);
}

void
Angle::operator+=(const Angle& rhs)
{
  mRad += rhs.mRad;
}

void
Angle::operator-=(const Angle& rhs)
{
  mRad -= rhs.mRad;
}

void
Angle::operator*=(float rhs)
{
  mRad *= rhs;
}

void
Angle::operator/=(float rhs)
{
  mRad /= rhs;
}

Angle
Angle::operator-() const
{
  return Angle::FromRadians(-mRad);
}

float
Sin(const Angle& ang)
{
  return std::sin(ang.InRadians());
}

float
Cos(const Angle& ang)
{
  return std::cos(ang.InRadians());
}

float
Tan(const Angle& ang)
{
  return std::tan(ang.InRadians());
}

Angle
Asin(float v)
{
  ASSERT(
      v <= 1 &&
      "v must be smaller than 1, use Limmit or Max on the value to not "
      "trigger this Assert");
  ASSERT(
      v >= -1 &&
      "v must be greater than -1, use Limmit or Min on the value to not "
      "trigger this Assert");
  return Angle::FromRadians(std::asin(v));
}

Angle
Acos(float v)
{
  ASSERT(
      v <= 1 &&
      "v must be smaller than 1, use Limmit or Max on the value to not "
      "trigger this Assert");
  ASSERT(
      v >= -1 &&
      "v must be greater than -1, use Limmit or Min on the value to not "
      "trigger this Assert");
  return Angle::FromRadians(std::acos(v));
}

Angle
Atan(float v)
{
  return Angle::FromRadians(std::atan(v));
}

Angle
Angle::GetWrapped() const
{
  Angle temp = *this;
  temp.Wrap();
  return temp;
}

Angle
operator+(const Angle& lhs, const Angle& rhs)
{
  Angle temp(lhs);
  temp += rhs;
  return temp;
}

Angle
operator-(const Angle& lhs, const Angle& rhs)
{
  Angle temp(lhs);
  temp -= rhs;
  return temp;
}

Angle
operator/(const Angle& lhs, float rhs)
{
  Angle temp(lhs);
  temp /= rhs;
  return temp;
}

Angle operator*(const Angle& lhs, float rhs)
{
  Angle temp(lhs);
  temp *= rhs;
  return temp;
}

Angle operator*(float rhs, const Angle& lhs)
{
  return lhs * rhs;
}

std::ostream&
operator<<(std::ostream& stream, const Angle& a)
{
  return stream << a.InDegrees() << " deg";
}

Angle
AngleTransform::Transform(const Angle& from, float v, const Angle& to)
{
  return Angle::FromRadians(Lerp(from.InRadians(), v, to.InRadians()));
}
