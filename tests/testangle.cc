#include "gtest/gtest.h"
#include "core/angle.h"
#include "core/numeric.h"

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
  auto a = Angle::FromDegrees(90);
  a += Angle::FromRadians(Pi()/2.0f);
  EXPECT_FLOAT_EQ(180.0f, a.inDegrees());
  EXPECT_FLOAT_EQ(180.0f, (Angle::FromDegrees(90) + Angle::FromRadians(Pi()/2.0f)).inDegrees());
}

GTEST(sub) {
  auto a = Angle::FromDegrees(180);
  a -= Angle::FromRadians(Pi()/2.0f);
  EXPECT_FLOAT_EQ(90.0f, a.inDegrees());
  EXPECT_FLOAT_EQ(90.0f, (Angle::FromDegrees(180) - Angle::FromRadians(Pi()/2.0f)).inDegrees());
}

GTEST(multi) {
  auto a = Angle::FromDegrees(90);
  a *= 2.0f;
  EXPECT_FLOAT_EQ(180.0f, a.inDegrees());
  EXPECT_FLOAT_EQ(180.0f, (Angle::FromDegrees(90) * 2.0f).inDegrees());
  EXPECT_FLOAT_EQ(180.0f, (2.0f * Angle::FromDegrees(90)).inDegrees());
}

struct KnownConstantsFromWikipedia : public ::testing::Test {
  // https://en.wikipedia.org/wiki/Trigonometric_functions#Explicit_values
  // https://en.wikipedia.org/wiki/Trigonometric_constants_expressed_in_real_radicals#Table_of_some_common_angles
  const float a = (Sqrt(6.0f) - Sqrt(2.0f)) / 4.0f;
  const float b = (Sqrt(2.0f) + Sqrt(6.0f)) / 4.0f;
};

const float NEAR  = 0.0000001f;
const float NEAR2 = 0.000001f;
const float NEAR3 = 0.00001f;

TEST_F(KnownConstantsFromWikipedia, sin) {
  EXPECT_NEAR(0.0f, Sin(Angle::FromDegrees(0.0f)), NEAR);
  EXPECT_NEAR(1.0f, Sin(Angle::FromDegrees(90.0f)), NEAR);
  EXPECT_NEAR(0.0f, Sin(Angle::FromDegrees(180.0f)), NEAR);
  EXPECT_NEAR(0.5f, Sin(Angle::FromDegrees(30.0f)), NEAR);
  EXPECT_NEAR(a, Sin(Angle::FromDegrees(15.0f)), NEAR);
  EXPECT_NEAR(b, Sin(Angle::FromDegrees(75.0f)), NEAR);
}

TEST_F(KnownConstantsFromWikipedia, cos) {
  EXPECT_NEAR(1.0f, Cos(Angle::FromDegrees(0.0f)), NEAR);
  EXPECT_NEAR(0.0f, Cos(Angle::FromDegrees(90.0f)), NEAR);
  EXPECT_NEAR(-1.0f, Cos(Angle::FromDegrees(180.0f)), NEAR);
  EXPECT_NEAR(0.5f, Cos(Angle::FromDegrees(60.0f)), NEAR);
  EXPECT_NEAR(b, Cos(Angle::FromDegrees(15.0f)), NEAR);
  EXPECT_NEAR(a, Cos(Angle::FromDegrees(75.0f)), NEAR);
}

GTEST(tan) {
  EXPECT_NEAR(0.0f, Tan(Angle::FromDegrees(0.0f)), NEAR);
  EXPECT_NEAR(1.0f, Tan(Angle::FromDegrees(45.0f)), NEAR);
  EXPECT_NEAR(0.0f, Tan(Angle::FromDegrees(180.0f)), NEAR);
  EXPECT_NEAR(1.0f, Tan(Angle::FromDegrees(225.0f)), NEAR2);
  EXPECT_NEAR(Sqrt(3.0f), Tan(Angle::FromDegrees(60.0f)), NEAR2);
}

TEST_F(KnownConstantsFromWikipedia, asin) {
  EXPECT_NEAR(0.0f, Asin(0.0f).inDegrees(), NEAR);
  EXPECT_NEAR(90.0f, Asin(1.0f).inDegrees(), NEAR);
  EXPECT_NEAR(30.0f, Asin(0.5f).inDegrees(), NEAR);
  EXPECT_NEAR(15.0f, Asin(a).inDegrees(), NEAR2);
  EXPECT_NEAR(75.0f, Asin(b).inDegrees(), NEAR3);
}

TEST_F(KnownConstantsFromWikipedia, acos) {
  EXPECT_NEAR(0.0f, Acos(1.0f).inDegrees(), NEAR);
  EXPECT_NEAR(90.0f, Acos(0.0f).inDegrees(), NEAR);
  EXPECT_NEAR(180.0f, Acos(-1.0f).inDegrees(), NEAR);
  EXPECT_NEAR(60.0f, Acos(0.5f).inDegrees(), NEAR);
  EXPECT_NEAR(15.0f, Acos(b).inDegrees(), NEAR3);
  EXPECT_NEAR(75.0f, Acos(a).inDegrees(), NEAR3);
}

GTEST(atan) {
  EXPECT_NEAR(0, Atan(0.0f).inDegrees(), NEAR);
  EXPECT_NEAR(45.0f, Atan(1.0f).inDegrees(), NEAR);
  EXPECT_NEAR(60.0f, Atan(Sqrt(3.0f)).inDegrees(), NEAR2);
}