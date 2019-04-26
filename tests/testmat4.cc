#include "core/mat4.h"
#include "core/numeric.h"

#include "tests/approx_equal.h"

#include "catch.hpp"

TEST_CASE("mat4-colmajor", "[mat]")
{
  const auto m =
      mat4i::FromColMajor(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
  REQUIRE(vec4i(0, 1, 2, 3) == m.GetColumn(0));
  REQUIRE(vec4i(4, 5, 6, 7) == m.GetColumn(1));
  REQUIRE(vec4i(8, 9, 10, 11) == m.GetColumn(2));
  REQUIRE(vec4i(12, 13, 14, 15) == m.GetColumn(3));
}

TEST_CASE("mat4-rowmajor", "[mat]")
{
  const auto m =
      mat4i::FromRowMajor(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
  REQUIRE(vec4i(0, 4, 8, 12) == m.GetColumn(0));
  REQUIRE(vec4i(1, 5, 9, 13) == m.GetColumn(1));
  REQUIRE(vec4i(2, 6, 10, 14) == m.GetColumn(2));
  REQUIRE(vec4i(3, 7, 11, 15) == m.GetColumn(3));
}

TEST_CASE("mat4-rowmajor_row", "[mat]")
{
  const auto m =
      mat4i::FromRowMajor(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
  REQUIRE(vec4i(0, 1, 2, 3) == m.GetRow(0));
  REQUIRE(vec4i(4, 5, 6, 7) == m.GetRow(1));
  REQUIRE(vec4i(8, 9, 10, 11) == m.GetRow(2));
  REQUIRE(vec4i(12, 13, 14, 15) == m.GetRow(3));
}

TEST_CASE("mat4-identity", "[mat]")
{
  const auto m =
      mat4i::FromRowMajor(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
  const auto i = mat4i::Identity();
  REQUIRE(i.GetColumn(0) == m.GetColumn(0));
  REQUIRE(i.GetColumn(1) == m.GetColumn(1));
  REQUIRE(i.GetColumn(2) == m.GetColumn(2));
  REQUIRE(i.GetColumn(3) == m.GetColumn(3));
}

TEST_CASE("mat4-index", "[mat]")
{
  const auto m =
      mat4i::FromRowMajor(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
  REQUIRE(3 == m(0, 3));
  REQUIRE(15 == m(3, 3));
}

TEST_CASE("mat4-major", "[mat]")
{
  const auto m =
      mat4i::FromRowMajor(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15)
          .GetMajor();
  REQUIRE(vec4i(0, 5, 10, 15) == m);
}


TEST_CASE("mat4-transposed", "[mat]")
{
  const auto m =
      mat4i::FromRowMajor(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15)
          .GetTransposed();
  REQUIRE(
      m == mat4i::FromColMajor(
               0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15));
}

TEST_CASE("mat4-axis", "[mat]")
{
  const auto m = mat4i::Identity();
  REQUIRE(vec3i(1, 0, 0) == static_cast<vec3i>(m.GetXAxis()));
  REQUIRE(vec3i(0, 1, 0) == m.GetYAxis());
  REQUIRE(vec3i(0, 0, 1) == m.GetZAxis());
}

TEST_CASE("mat4-multiply", "[mat]")
{
  const auto m =
      mat4i::FromRowMajor(
          0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15) *
      mat4i::FromRowMajor(
          16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
  // thanks wolfram alpha for the result
  // http://www.wolframalpha.com/widgets/view.jsp?id=cc71c2e95a80c217564d530fd8297b0e
  // simplify({{0,1,2,3},{4,5,6,7},{8,9,10,11},{12,13,14,15}}.{{16,17,18,19},{20,21,22,23},{24,25,26,27},{28,29,30,31}})
  REQUIRE(
      m == mat4i::FromRowMajor(
               152,
               158,
               164,
               170,
               504,
               526,
               548,
               570,
               856,
               894,
               932,
               970,
               1208,
               1262,
               1316,
               1370));
}

TEST_CASE("mat4-add", "[mat]")
{
  const auto m =
      mat4i::FromRowMajor(
          0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15) +
      mat4i::FromRowMajor(
          16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
  REQUIRE(
      m == mat4i::FromRowMajor(
               16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46));
}

TEST_CASE("mat4-sub", "[mat]")
{
  const auto m =
      mat4i::FromRowMajor(
          16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31) -
      mat4i::FromRowMajor(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
  REQUIRE(
      m == mat4i::FromRowMajor(
               16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16));
}


TEST_CASE("mat4-mat3", "[mat]")
{
  const auto m4 =
      mat4i::FromColMajor(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
  const auto m3 = m4.GetMat3();
  REQUIRE(std::make_tuple(0, 1, 2) == m3.GetColumn(0));
  REQUIRE(std::make_tuple(4, 5, 6) == m3.GetColumn(1));
  REQUIRE(std::make_tuple(8, 9, 10) == m3.GetColumn(2));
}


TEST_CASE("mat4-inverse", "[mat]")
{
  const auto Mat0 = mat4f::FromColMajor(
      0.6f,
      0.2f,
      0.3f,
      0.4f,
      0.2f,
      0.7f,
      0.5f,
      0.3f,
      0.3f,
      0.5f,
      0.7f,
      0.2f,
      0.4f,
      0.3f,
      0.2f,
      0.6f);

  auto Inv0 = Mat0;  // glm::inverse(Mat0);
  REQUIRE(Inv0.Invert());
  REQUIRE(Inv0.Invert());

  // EXPECT_EQ(Mat0, Inv0);
}


////////////////////////////////////////////////////////////////////////////////

TEST_CASE("mat4-TestTransformation", "[mat]")
{
  REQUIRE(
      vec3i(1, 2, 3) ==
      mat4i::FromTranslation(vec3i(1, 2, 3)).GetTranslation());
}


TEST_CASE("mat4-TestRight", "[mat]")
{
  REQUIRE(unit3i::XAxis() == mat4i::Identity().GetXAxis());
}


TEST_CASE("mat4-TestUp", "[mat]")
{
  REQUIRE(unit3i::YAxis() == mat4i::Identity().GetYAxis());
}


TEST_CASE("mat4-TestIn", "[mat]")
{
  REQUIRE(unit3i::ZAxis() == mat4i::Identity().GetZAxis());
}

TEST_CASE("mat4-test", "[mat]")
{
  const mat4f start = mat4f::Identity();
  AxisAngle   aa =
      AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(-90));
  const point3f toTransform(0, 0, -5);
  const point3f result(5, 0, 0);

  SECTION("TestRotationAxisAngle")
  {
    const auto r = start.Rotate(aa).GetTransform(toTransform);
    REQUIRE(r == approx(result));
  }
}


TEST_CASE("mat4-TestCombined_RT", "[mat]")
{
  const auto r = mat4f::Identity()
                     .Rotate(AxisAngle::RightHandAround(
                         unit3f::Up(), Angle::FromDegrees(-90)))
                     .Translate(vec3f(0, 0, -5))
                     .GetTransform(point3f(0, 0, 0));
  REQUIRE(r == approx(point3f(5, 0, 0)));
}

TEST_CASE("mat4-TestCombined2_RT", "[mat]")
{
  const auto r = mat4f::Identity()
                     .Rotate(AxisAngle::RightHandAround(
                         unit3f::Up(), Angle::FromDegrees(90)))
                     .Translate(vec3f(0, 0, -5))
                     .GetTransform(point3f(0, 0, 0));
  REQUIRE(r == approx(point3f(-5, 0, 0)));
}

TEST_CASE("mat4-TestCombined_TR", "[mat]")
{
  const auto r = mat4f::Identity()
                     .Translate(vec3f(0, 0, 5))
                     .Rotate(AxisAngle::RightHandAround(
                         unit3f::Up(), Angle::FromDegrees(-90)))
                     .GetTransform(point3f(0, 0, 0));
  REQUIRE(r == approx(point3f(0, 0, 5)));
}

TEST_CASE("mat4-TestTranslation", "[mat]")
{
  const auto r =
      mat4i::Identity().Translate(vec3i(1, 2, 3)).GetTransform(point3i(7, 8, 9));
  REQUIRE(r == point3i(8, 10, 12));
}


TEST_CASE("mat4-TestIentityTransform", "[mat]")
{
  const auto r = mat4i::Identity().GetTransform(point3i(1, 2, 3));
  REQUIRE(r == point3i(1, 2, 3));
}

TEST_CASE("mat4-TestIentityMultiply", "[mat]")
{
  REQUIRE(mat4i::Identity() * mat4i::Identity() == mat4i::Identity());
}

TEST_CASE("mat4-TestVec4Multiply", "[mat]")
{
  const auto m = mat4i::FromRowMajor(
                     0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15) *
                 vec4i(16, 17, 18, 19);
  // simplify({{0,1,2,3},{4,5,6,7},{8,9,10,11},{12,13,14,15}}.{16,17,18,19})
  REQUIRE(m == vec4i(110, 390, 670, 950));
}
