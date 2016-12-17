#include "gtest/gtest.h"
#include "core/quat.h"
#include "tests/testbase.h"

#define GTEST(X) TEST(quat, X)

GTEST(identity) {
  const auto q = quati::Identity();
  EXPECT_EQ(quati(1,vec3i(0,0,0)), q);
}

GTEST(testVecOp)
{
  EXPECT_EQ(vec3i(1, 2, 3), quati(4, vec3i(1, 2, 3)).vec());
}

GTEST(testLocalAxis)
{
  quatf q = quatf::Identity();
  EXPECT_PRED_FORMAT2(almost_equal, vec3f::Right(), q.Right());
  EXPECT_PRED_FORMAT2(almost_equal, vec3f::Up(), q.Up());
  EXPECT_PRED_FORMAT2(almost_equal, vec3f::In(), q.In());

  q = quatf(AxisAngle::RightHandAround(vec3f::Up(), Angle::FromDegrees(90)));
  EXPECT_PRED_FORMAT2(almost_equal, vec3f::In(), q.Right());
  EXPECT_PRED_FORMAT2(almost_equal, vec3f::Up(), q.Up());
  EXPECT_PRED_FORMAT2(almost_equal, vec3f::Left(), q.In());

  q = quatf(AxisAngle::RightHandAround(vec3f::Right(), Angle::FromDegrees(90)));
  EXPECT_PRED_FORMAT2(almost_equal, vec3f::Right(), q.Right());
  EXPECT_PRED_FORMAT2(almost_equal, vec3f::Out(), q.Up());
  EXPECT_PRED_FORMAT2(almost_equal, vec3f::Up(), q.In());

  q = quatf(AxisAngle::RightHandAround(vec3f::In(), Angle::FromDegrees(90)));
  EXPECT_PRED_FORMAT2(almost_equal, vec3f::Down(), q.Right());
  EXPECT_PRED_FORMAT2(almost_equal, vec3f::Right(), q.Up());
  EXPECT_PRED_FORMAT2(almost_equal, vec3f::In(), q.In());
}


GTEST(testLook)
{
  EXPECT_PRED_FORMAT2(almost_equal_quat, quatf(AxisAngle::RightHandAround(vec3f::Up(), Angle::FromDegrees(-90))), quatf::LookAt(vec3f(0, 0, 0), vec3f(5, 0, 0), vec3f::Up()));
  EXPECT_PRED_FORMAT2(almost_equal_quat, quatf(AxisAngle::RightHandAround(vec3f::Up(), Angle::FromDegrees(-90))),  quatf::LookAt(vec3f(0, 0, 0), vec3f(2, 0, 0), vec3f::Up()));
  EXPECT_PRED_FORMAT2(almost_equal_quat, quatf(AxisAngle::RightHandAround(vec3f::Up(), Angle::FromDegrees(-90))),  quatf::LookAt(vec3f(0, 0, 0), vec3f(0.25f, 0, 0), vec3f::Up()));

  // EXPECT_PRED_FORMAT2(almost_equal_quat, quatf(AxisAngle::RightHandAround(vec3f::Up(), Angle::FromDegrees(180))), quatf::LookInDirection(vec3f(0, 0, -9), vec3f::Up()));
  // EXPECT_PRED_FORMAT2(almost_equal_quat, quatf(AxisAngle::RightHandAround(vec3f::Up(), Angle::FromDegrees(-90))), quatf::LookInDirection(vec3f(3, 0, 0), vec3f::Up()));
  // EXPECT_PRED_FORMAT2(almost_equal_quat, quatf(AxisAngle::RightHandAround(vec3f::Up(), Angle::FromDegrees(90))),  quatf::LookInDirection(vec3f(-5, 0, 0), vec3f::Up()));

  // todo: add more test where up != Up()
}
