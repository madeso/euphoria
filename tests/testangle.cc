#include "gtest/gtest.h"
#include "spacetyper/angle.h"
#include "spacetyper/numeric.h"

#define GTEST(X) TEST(angle, X)

GTEST(constructor_degrees) {
  const auto a = Angle::FromDegrees(180);
  EXPECT_FLOAT_EQ(180, a.inDegrees());
}

GTEST(constructor_degrees_pi) {
  const auto a = Angle::FromDegrees(180);
  EXPECT_FLOAT_EQ(Pi(), a.inRadians());
}

GTEST(zero) {
  const auto a = Angle::Zero();
  EXPECT_FLOAT_EQ(0.0f, a.inDegrees());
}

GTEST(constructor_radians) {
  const auto a = Angle::FromRadians(Pi());
  EXPECT_FLOAT_EQ(180, a.inDegrees());
}

GTEST(constructor_percent) {
  const auto a = Angle::FromPercentOf360(0.5f);
  EXPECT_FLOAT_EQ(180, a.inDegrees());
}

GTEST(wrapped) {
  const auto a = Angle::FromDegrees(360+90).GetWrapped();
  EXPECT_FLOAT_EQ(90.0f, a.inDegrees());
}

GTEST(wrap) {
  auto a = Angle::FromDegrees(360+90);
  EXPECT_FLOAT_EQ(360.0f + 90.0f, a.inDegrees());
  a.Wrap();
  EXPECT_FLOAT_EQ(90.0f, a.inDegrees());
}

GTEST(add) {
  const auto a = Angle::FromDegrees(90) + Angle::FromRadians(Pi()/2.0f);
  EXPECT_FLOAT_EQ(180.0f, a.inDegrees());
}

GTEST(sub) {
  const auto a = Angle::FromDegrees(180) - Angle::FromRadians(Pi()/2.0f);
  EXPECT_FLOAT_EQ(90.0f, a.inDegrees());
}

GTEST(multi) {
  const auto a = Angle::FromDegrees(90) * 2.0f;
  EXPECT_FLOAT_EQ(180.0f, a.inDegrees());
}
