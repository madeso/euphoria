#include "core/quat.h"
#include "tests/approx_equal.h"

#include "catch.hpp"

TEST_CASE("quat-identity", "[quat]")
{
  const auto q = quati::Identity();
  REQUIRE(q == quati(1, vec3i(0, 0, 0)));
}

TEST_CASE("quat-testVecOp", "[quat]")
{
  REQUIRE(quati(4, vec3i(1, 2, 3)).vec() == vec3i(1, 2, 3));
}

#define EXPECT_PRED_FORMAT2(a, b) REQUIRE(b == approx(a))

TEST_CASE("quat-testLocalAxis", "[quat]")
{
  quatf q = quatf::Identity();
  EXPECT_PRED_FORMAT2(unit3f::Right(), q.Right());
  EXPECT_PRED_FORMAT2(unit3f::Up(), q.Up());
  EXPECT_PRED_FORMAT2(unit3f::In(), q.In());

  q = quatf::FromAxisAngle(
      AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(90)));
  EXPECT_PRED_FORMAT2(unit3f::In(), q.Right());
  EXPECT_PRED_FORMAT2(unit3f::Up(), q.Up());
  EXPECT_PRED_FORMAT2(unit3f::Left(), q.In());

  q = quatf::FromAxisAngle(
      AxisAngle::RightHandAround(unit3f::Right(), Angle::FromDegrees(90)));
  EXPECT_PRED_FORMAT2(unit3f::Right(), q.Right());
  EXPECT_PRED_FORMAT2(unit3f::Out(), q.Up());
  EXPECT_PRED_FORMAT2(unit3f::Up(), q.In());

  q = quatf::FromAxisAngle(
      AxisAngle::RightHandAround(unit3f::In(), Angle::FromDegrees(90)));
  EXPECT_PRED_FORMAT2(unit3f::Down(), q.Right());
  EXPECT_PRED_FORMAT2(unit3f::Right(), q.Up());
  EXPECT_PRED_FORMAT2(unit3f::In(), q.In());
}


TEST_CASE("quat-testLook", "[quat]")
{
  EXPECT_PRED_FORMAT2(
      quatf::FromAxisAngle(
          AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(-90))),
      quatf::LookAt(vec3f(0, 0, 0), vec3f(5, 0, 0), unit3f::Up()));
  EXPECT_PRED_FORMAT2(
      quatf::FromAxisAngle(
          AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(-90))),
      quatf::LookAt(vec3f(0, 0, 0), vec3f(2, 0, 0), unit3f::Up()));
  EXPECT_PRED_FORMAT2(
      quatf::FromAxisAngle(
          AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(-90))),
      quatf::LookAt(vec3f(0, 0, 0), vec3f(0.25f, 0, 0), unit3f::Up()));

  // need to pass in a normalized vec3 or it won't compile, hence the
  // .GetNormalized() call
  // Z looks reversed, but remember, negative direction is in
  EXPECT_PRED_FORMAT2(
      quatf::FromAxisAngle(
          AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(0))),
      quatf::LookInDirection(vec3f(0, 0, -9).GetNormalized(), unit3f::Up()));
  EXPECT_PRED_FORMAT2(
      quatf::FromAxisAngle(
          AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(180))),
      quatf::LookInDirection(vec3f(0, 0, 9).GetNormalized(), unit3f::Up()));
  EXPECT_PRED_FORMAT2(
      quatf::FromAxisAngle(
          AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(-90))),
      quatf::LookInDirection(vec3f(3, 0, 0).GetNormalized(), unit3f::Up()));
  EXPECT_PRED_FORMAT2(
      quatf::FromAxisAngle(
          AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(90))),
      quatf::LookInDirection(vec3f(-5, 0, 0).GetNormalized(), unit3f::Up()));

  // todo: add more test where up != Up()
}


TEST_CASE("quat-testCombine", "[quat]")
{
  EXPECT_PRED_FORMAT2(
      quatf::FromAxisAngle(
          AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(90))),
      quatf::Identity().Rotate(quatf::FromAxisAngle(
          AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(90)))));

  EXPECT_PRED_FORMAT2(
      quatf::FromAxisAngle(
          AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(90))),
      quatf::FromAxisAngle(
          AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(90)))
          .Rotate(quatf::Identity()));

  EXPECT_PRED_FORMAT2(
      quatf::FromAxisAngle(
          AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(90))),
      quatf::Identity().Rotate(quatf::FromAxisAngle(
          AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(90)))));

  EXPECT_PRED_FORMAT2(

      quatf::FromAxisAngle(
          AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(-90)))
          .Rotate(quatf::FromAxisAngle(AxisAngle::RightHandAround(
              unit3f::Out(), Angle::FromDegrees(90)))),
      quatf::FromAxisAngle(
          AxisAngle::RightHandAround(unit3f::Right(), Angle::FromDegrees(90)))
          .Rotate(quatf::FromAxisAngle(AxisAngle::RightHandAround(
              unit3f::Up(), Angle::FromDegrees(-90)))));
}


TEST_CASE("quat-verifyTestAxisAngle", "[quat]")
{
  EXPECT_PRED_FORMAT2(

      AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(0)),
      AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(0)));
  EXPECT_PRED_FORMAT2(

      AxisAngle::RightHandAround(unit3f::Right(), Angle::FromDegrees(90)),
      AxisAngle::RightHandAround(unit3f::Right(), Angle::FromDegrees(90)));
  EXPECT_PRED_FORMAT2(

      AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(-45)),
      AxisAngle::RightHandAround(-unit3f::Up(), Angle::FromDegrees(45)));
  EXPECT_PRED_FORMAT2(

      AxisAngle::RightHandAround(unit3f::Right(), Angle::FromDegrees(90)),
      AxisAngle::RightHandAround(-unit3f::Right(), Angle::FromDegrees(-90)));
}

TEST_CASE("quat-checkAxisAngle", "[quat]")
{
  EXPECT_PRED_FORMAT2(

      AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(0)),
      quatf::FromAxisAngle(
          AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(0)))
          .ToAxisAngle());
  EXPECT_PRED_FORMAT2(

      AxisAngle::RightHandAround(unit3f::Right(), Angle::FromDegrees(0)),
      quatf::FromAxisAngle(
          AxisAngle::RightHandAround(unit3f::Right(), Angle::FromDegrees(0)))
          .ToAxisAngle());
  EXPECT_PRED_FORMAT2(

      AxisAngle::RightHandAround(unit3f::Right(), Angle::FromDegrees(90)),
      quatf::FromAxisAngle(
          AxisAngle::RightHandAround(unit3f::Right(), Angle::FromDegrees(90)))
          .ToAxisAngle());
  EXPECT_PRED_FORMAT2(

      AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(-45)),
      quatf::FromAxisAngle(
          AxisAngle::RightHandAround(-unit3f::Up(), Angle::FromDegrees(45)))
          .ToAxisAngle());
  EXPECT_PRED_FORMAT2(

      AxisAngle::RightHandAround(unit3f::Right(), Angle::FromDegrees(90)),
      quatf::FromAxisAngle(
          AxisAngle::RightHandAround(-unit3f::Right(), Angle::FromDegrees(-90)))
          .ToAxisAngle());
}

TEST_CASE("quat-checkQuatConjugate", "[quat]")
{
  const auto a = quatf::FromAxisAngle(
      AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(90)));
  const auto b =
      quatf::FromAxisAngle(
          AxisAngle::RightHandAround(unit3f::Up(), Angle::FromDegrees(-90)))
          .GetConjugate();
  EXPECT_PRED_FORMAT2(a, b);
}
