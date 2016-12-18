#include "core/angle.h"

#include "core/numeric.h"
#include <cmath>
#include <cassert>

namespace // local
{
const float rad2deg(const float rad)
{
  return (180.0f / Pi()) * rad;
}
const float deg2rad(const float deg)
{
  return Pi() / 180.0f * deg;
}
}

const float Angle::inDegrees() const
{
  return rad2deg(mRad);
}

const float Angle::inRadians() const
{
  return mRad;
}

const Angle Angle::FromDegrees(float deg)
{
  return Angle(deg2rad(deg));
}

const Angle Angle::FromRadians(float rad)
{
  return Angle(rad);
}

Angle::Angle(float rad)
    : mRad(rad)
{
}

void Angle::Wrap()
{
  mRad = ::Wrap(0.0f, mRad, Pi() * 2.0f);
}

void Angle::operator+=(const Angle& rhs)
{
  mRad += rhs.mRad;
}

void Angle::operator-=(const Angle& rhs)
{
  mRad -= rhs.mRad;
}

void Angle::operator*=(const float rhs)
{
  mRad *= rhs;
}

void Angle::operator/=(const float rhs)
{
  mRad /= rhs;
}

const float Sin(const Angle& ang)
{
  return std::sin(ang.inRadians());
}

const float Cos(const Angle& ang)
{
  return std::cos(ang.inRadians());
}

const float Tan(const Angle& ang)
{
  return std::tan(ang.inRadians());
}

const Angle Asin(const float v)
{
  assert(v <= 1 && "v must be smaller than 1, use Limmit or Max on the value to not trigger this Assert");
  assert(v >= -1 && "v must be greater than -1, use Limmit or Min on the value to not trigger this Assert");
  return Angle::FromRadians(std::asin(v));
}

const Angle Acos(const float v)
{
  assert(v <= 1 && "v must be smaller than 1, use Limmit or Max on the value to not trigger this Assert");
  assert(v >= -1 && "v must be greater than -1, use Limmit or Min on the value to not trigger this Assert");
  return Angle::FromRadians(std::acos(v));
}

const Angle Atan(const float v)
{
  return Angle::FromRadians(std::atan(v));
}

const Angle Angle::GetWrapped() const
{
  Angle temp = *this;
  temp.Wrap();
  return temp;
}

const Angle operator+(const Angle& lhs, const Angle& rhs)
{
  Angle temp(lhs);
  temp += rhs;
  return temp;
}

const Angle operator-(const Angle& lhs, const Angle& rhs)
{
  Angle temp(lhs);
  temp -= rhs;
  return temp;
}

const Angle operator/(const Angle& lhs, const float rhs)
{
  Angle temp(lhs);
  temp /= rhs;
  return temp;
}

const Angle operator*(const Angle& lhs, const float rhs)
{
  Angle temp(lhs);
  temp *= rhs;
  return temp;
}

const Angle operator*(const float rhs, const Angle& lhs)
{
  return lhs * rhs;
}

const Angle Angle::FromPercentOf360(const float percent)
{
  return Angle::FromRadians(percent * Pi() * 2.0f);
}

const Angle Angle::Zero()
{
  return Angle::FromRadians(0);
}

std::ostream& operator<<(std::ostream& stream, const Angle& a)
{
  return stream << a.inDegrees() << " deg";
}

