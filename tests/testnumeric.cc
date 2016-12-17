#include "gtest/gtest.h"
#include "core/numeric.h"

#define GTEST(X) TEST(numeric, X)

GTEST(is_equal) {
  EXPECT_TRUE(IsEqual(1.2f, 1.2f));
  EXPECT_TRUE(IsEqual(21.12f, 21.12f));
  EXPECT_FALSE(IsEqual(2.1f, 1.2f));
}

GTEST(is_zero) {
  EXPECT_TRUE(IsZero(0.000001f));
  EXPECT_TRUE(IsZero(-0.000001f));
  EXPECT_FALSE(IsZero(1.2f));
}

GTEST(zero_or_value) {
  EXPECT_EQ(0.0f, ZeroOrValue(0.000001f));
  EXPECT_EQ(12.0f, ZeroOrValue(12.0f));
}

GTEST(sign) {
  EXPECT_EQ(-1, Sign(-12.0f));
  EXPECT_EQ(1, Sign(32.0f));
  EXPECT_EQ(1, Sign(0.0f));
}

GTEST(lerp) {
  EXPECT_FLOAT_EQ(1.0f, Lerp(0.0f, 0.5f, 2.0f));
  EXPECT_FLOAT_EQ(0.0f, Lerp(-1.0f, 0.5f, 1.0f));
  EXPECT_FLOAT_EQ(1.0f, Lerp(0.0f, 0.25f, 4.0f));
}

GTEST(square) {
  EXPECT_FLOAT_EQ(9.0f, Square(3.0f));
}

GTEST(sqrt) {
  EXPECT_FLOAT_EQ(3.0f, Sqrt(9.0f));
}

GTEST(abs) {
  EXPECT_FLOAT_EQ(34.0f, Abs(34.0f));
  EXPECT_FLOAT_EQ(14.0f, Abs(-14.0f));
}

GTEST(min) {
  EXPECT_FLOAT_EQ(1.0f, Min(1.0f, 2.0f));
  EXPECT_FLOAT_EQ(-2.0f, Min(1.0f, -2.0f));
}

GTEST(max) {
  EXPECT_FLOAT_EQ(2.0f, Max(1.0f, 2.0f));
  EXPECT_FLOAT_EQ(1.0f, Max(1.0f, -2.0f));
}

GTEST(to01) {
  EXPECT_FLOAT_EQ(0.5f, To01(1.0f, 2.0f, 3.0f));
  EXPECT_FLOAT_EQ(0.5f, To01(-1.0f, 0.0f, 1.0f));
  EXPECT_FLOAT_EQ(1.5f, To01(-1.0f, 2.0f, 1.0f));
}

GTEST(from01) {
  EXPECT_FLOAT_EQ(1.0f, From01(0.0f, 0.5f, 2.0f));
  EXPECT_FLOAT_EQ(0.0f, From01(-2.0f, 0.5f, 2.0f));
  EXPECT_FLOAT_EQ(4.0f, From01(-2.0f, 1.5f, 2.0f));
}

GTEST(remap) {
  EXPECT_FLOAT_EQ(1.0f, Remap(0.0f, 3.0f, 1.5f, 0.0f, 2.0f));
}

GTEST(get360angular) {
  EXPECT_FLOAT_EQ(0.5f, Get360Angular(0.0f, 0.25f, 1.0f));
  EXPECT_FLOAT_EQ(0.5f, Get360Angular(0.0f, 0.75f, 1.0f));
  EXPECT_FLOAT_EQ(1.0f, Get360Angular(0.0f, 0.5f, 1.0f));
  EXPECT_FLOAT_EQ(0.0f, Get360Angular(0.0f, 0.0f, 1.0f));
  EXPECT_FLOAT_EQ(0.0f, Get360Angular(0.0f, 1.0f, 1.0f));
}

GTEST(keep_within) {
  EXPECT_FLOAT_EQ(0, KeepWithin(0, -4, 1));
  EXPECT_FLOAT_EQ(2, KeepWithin(0, 2, 4));
  EXPECT_FLOAT_EQ(4, KeepWithin(0, 8, 4));
}

GTEST(is_within) {
  EXPECT_FALSE(IsWithin(0, -4, 1));
  EXPECT_TRUE(IsWithin(0, 2, 4));
  EXPECT_FALSE(IsWithin(0, 8, 4));
  EXPECT_FALSE(IsWithin(0, 4, 4));
}

GTEST(is_within_inclusive) {
  EXPECT_FALSE(IsWithinInclusive(0, -4, 1));
  EXPECT_TRUE(IsWithinInclusive(0, 2, 4));
  EXPECT_FALSE(IsWithinInclusive(0, 8, 4));
  EXPECT_TRUE(IsWithinInclusive(0, 4, 4));
}

GTEST(wrap) {
  EXPECT_FLOAT_EQ(0.5f, Wrap(0.0f, 0.5f, 1.0f));
  EXPECT_FLOAT_EQ(0.5f, Wrap(0.0f, 1.5f, 1.0f));
  EXPECT_FLOAT_EQ(0.5f, Wrap(0.0f, 55.5f, 1.0f));

  EXPECT_FLOAT_EQ(-0.5f, Wrap(-1.0f, 1.5f, 1.0f));
}

GTEST(wrapi) {
  EXPECT_EQ(2, Wrapi(0, 2, 4));
  EXPECT_EQ(1, Wrapi(0, 6, 4));
  EXPECT_EQ(1, Wrapi(0, 9, 1));

  EXPECT_EQ(-3, Wrapi(-4, 6, 4));
}

GTEST(round) {
  EXPECT_FLOAT_EQ(1.0f, Round(0.9f, 1.0f));
  EXPECT_FLOAT_EQ(1.5f, Round(1.493f, 0.5f));
  EXPECT_FLOAT_EQ(2.0f, Round(2.9f, 2.0f));
  EXPECT_FLOAT_EQ(3.0f, Round(2.9f, 1.0f));
}

GTEST(increment_and_wrap) {
  float f = 8.0f;
  EXPECT_EQ(0, IncrementAndWrap(0.0f, &f, 1.0f, 10.0f));
  EXPECT_FLOAT_EQ(9.0f, f);
  EXPECT_EQ(0, IncrementAndWrap(0.0f, &f, 1.0f, 10.0f));
  EXPECT_FLOAT_EQ(10.0f, f);
  EXPECT_EQ(1, IncrementAndWrap(0.0f, &f, 1.0f, 10.0f));
  EXPECT_FLOAT_EQ(1.0f, f);
}

GTEST(increment_and_wrapi) {
  int f = 8.0f;
  EXPECT_EQ(0, IncrementAndWrapi(0, &f, 1, 10));
  EXPECT_EQ(9, f);
  EXPECT_EQ(0, IncrementAndWrapi(0, &f, 1, 10));
  EXPECT_EQ(10, f);
  EXPECT_EQ(1, IncrementAndWrapi(0, &f, 1, 10));
  EXPECT_EQ(0, f);
}
