#include "core/angle.h"
#include "core/numeric.h"

#include "catch.hpp"
#include "tests/approx.h"

// todo: don't use horrible define to use out custom Approx
#define Approx custom::Approx<float>

TEST_CASE("angle-constructor_degrees", "[angle]")
{
  const auto a = Angle::FromDegrees(180);
  REQUIRE(a.InDegrees() == Approx(180.0f));
}

TEST_CASE("angle-constructor_degrees_pi", "[angle]")
{
  const auto a = Angle::FromDegrees(180);
  REQUIRE(a.InRadians() == Approx(Pi()));
}

TEST_CASE("angle-zero", "[angle]")
{
  const auto a = 0.0_rad;
  REQUIRE(a.InDegrees() == Approx(0.0f));
}

TEST_CASE("angle-constructor_radians", "[angle]")
{
  const auto a = Angle::FromRadians(Pi());
  REQUIRE(a.InDegrees() == Approx(180.0f));
}

TEST_CASE("angle-constructor_percent", "[angle]")
{
  const auto a = Angle::FromPercentOf360(0.5f);
  REQUIRE(a.InDegrees() == Approx(180.0f));
}

TEST_CASE("angle-wrapped", "[angle]")
{
  const auto a = Angle::FromDegrees(360 + 90).GetWrapped();
  REQUIRE(a.InDegrees() == Approx(90.0f));
}

TEST_CASE("angle-wrap", "[angle]")
{
  auto a = Angle::FromDegrees(360 + 90);
  REQUIRE(a.InDegrees() == Approx(360.0f + 90.0f));
  a.Wrap();
  REQUIRE(a.InDegrees() == Approx(90.0f));
}

TEST_CASE("angle-add", "[angle]")
{
  auto a = Angle::FromDegrees(90);
  a += Angle::FromRadians(Pi() / 2.0f);
  REQUIRE(a.InDegrees() == Approx(180.0f));
  REQUIRE(
      (Angle::FromDegrees(90) + Angle::FromRadians(Pi() / 2.0f)).InDegrees() ==
      Approx(180.0f));
}

TEST_CASE("angle-sub", "[angle]")
{
  auto a = Angle::FromDegrees(180);
  a -= Angle::FromRadians(Pi() / 2.0f);
  REQUIRE(a.InDegrees() == Approx(90.0f));
  REQUIRE(
      (Angle::FromDegrees(180) - Angle::FromRadians(Pi() / 2.0f)).InDegrees() ==
      Approx(90.0f));
}

TEST_CASE("angle-multi", "[angle]")
{
  auto a = Angle::FromDegrees(90);
  a *= 2.0f;
  REQUIRE(a.InDegrees() == Approx(180.0f));
  REQUIRE((Angle::FromDegrees(90) * 2.0f).InDegrees() == Approx(180.0f));
  REQUIRE((2.0f * Angle::FromDegrees(90)).InDegrees() == Approx(180.0f));
}

TEST_CASE("angle-div", "[angle]")
{
  auto a = Angle::FromDegrees(180);
  a /= 2;
  REQUIRE(a.InDegrees() == Approx(90.0f));
  REQUIRE((Angle::FromDegrees(180) / 2.0f).InDegrees() == Approx(90.0f));
}

TEST_CASE("angle-atan", "[angle]")
{
  REQUIRE(Atan(0.0f).InDegrees() == Approx(0.0f));
  REQUIRE(Atan(1.0f).InDegrees() == Approx(45.0f));
  REQUIRE(Atan(Sqrt(3.0f)).InDegrees() == Approx(60.0f));
}

TEST_CASE("angle-tan", "[angle]")
{
  REQUIRE(Tan(Angle::FromDegrees(0.0f)) == Approx(0.0f));
  REQUIRE(Tan(Angle::FromDegrees(45.0f)) == Approx(1.0f));
  REQUIRE(Tan(Angle::FromDegrees(180.0f)) == Approx(0.0f));
  REQUIRE(Tan(Angle::FromDegrees(225.0f)) == Approx(1.0f));
  REQUIRE(Tan(Angle::FromDegrees(60.0f)) == Approx(Sqrt(3.0f)));
}

TEST_CASE("angle-Wikipedia constants", "[angle]")
{
  // https://en.wikipedia.org/wiki/Trigonometric_functions#Explicit_values
  // https://en.wikipedia.org/wiki/Trigonometric_constants_expressed_in_real_radicals#Table_of_some_common_angles
  const float a = (Sqrt(6.0f) - Sqrt(2.0f)) / 4.0f;
  const float b = (Sqrt(2.0f) + Sqrt(6.0f)) / 4.0f;

  SECTION("sin")
  {
    REQUIRE(Sin(Angle::FromDegrees(0.0f)) == Approx(0.0f));
    REQUIRE(Sin(Angle::FromDegrees(90.0f)) == Approx(1.0f));
    REQUIRE(Sin(Angle::FromDegrees(180.0f)) == Approx(0.0f));
    REQUIRE(Sin(Angle::FromDegrees(30.0f)) == Approx(0.5f));
    REQUIRE(Sin(Angle::FromDegrees(15.0f)) == Approx(a));
    REQUIRE(Sin(Angle::FromDegrees(75.0f)) == Approx(b));
  }

  SECTION("cos")
  {
    REQUIRE(Cos(Angle::FromDegrees(0.0f)) == Approx(1.0f));
    REQUIRE(Cos(Angle::FromDegrees(90.0f)) == Approx(0.0f));
    REQUIRE(Cos(Angle::FromDegrees(180.0f)) == Approx(-1.0f));
    REQUIRE(Cos(Angle::FromDegrees(60.0f)) == Approx(0.5f));
    REQUIRE(Cos(Angle::FromDegrees(15.0f)) == Approx(b));
    REQUIRE(Cos(Angle::FromDegrees(75.0f)) == Approx(a));
  }

  SECTION("asin")
  {
    REQUIRE(Asin(0.0f).InDegrees() == Approx(0.0f));
    REQUIRE(Asin(1.0f).InDegrees() == Approx(90.0f));
    REQUIRE(Asin(0.5f).InDegrees() == Approx(30.0f));
    REQUIRE(Asin(a).InDegrees() == Approx(15.0f));
    REQUIRE(Asin(b).InDegrees() == Approx(75.0f));
  }

  SECTION("acos")
  {
    REQUIRE(Acos(1.0f).InDegrees() == Approx(0.0f));
    REQUIRE(Acos(0.0f).InDegrees() == Approx(90.0f));
    REQUIRE(Acos(-1.0f).InDegrees() == Approx(180.0f));
    REQUIRE(Acos(0.5f).InDegrees() == Approx(60.0f));
    REQUIRE(Acos(b).InDegrees() == Approx(15.0f));
    REQUIRE(Acos(a).InDegrees() == Approx(75.0f));
  }
}
