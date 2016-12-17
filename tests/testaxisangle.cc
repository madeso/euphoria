#include "gtest/gtest.h"
#include "core/axisangle.h"

#define GTEST(X) TEST(axisangle, X)

GTEST(righthand) {
  const auto aa = AxisAngle::RightHandAround(vec3f::XAxis(), Angle::FromDegrees(12.0f));
  EXPECT_FLOAT_EQ(12.0f, aa.angle.inDegrees());
  EXPECT_FLOAT_EQ(1, aa.axis.x);
  EXPECT_FLOAT_EQ(0, aa.axis.y);
  EXPECT_FLOAT_EQ(0, aa.axis.z);
}