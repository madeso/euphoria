#include "gtest/gtest.h"
#include "spacetyper/vec2.h"

#define GTEST(X) TEST(vec2, X)

GTEST(constructor_both) {
  const auto v = vec2i(42);
  EXPECT_EQ(42, v.x);
  EXPECT_EQ(42, v.y);
}

GTEST(constructor_explicit) {
  const auto v = vec2i(10, 20);
  EXPECT_EQ(10, v.x);
  EXPECT_EQ(20, v.y);
}

GTEST(add) {
  const auto v = vec2i(10, 20) + vec2i(1, 2);
  EXPECT_EQ(11, v.x);
  EXPECT_EQ(22, v.y);
}

GTEST(add_assign) {
  auto v = vec2i(1, 2);
  v += vec2i(10, 20);
  EXPECT_EQ(11, v.x);
  EXPECT_EQ(22, v.y);
}


GTEST(sub) {
  const auto v = vec2i(11, 22) - vec2i(1, 2);
  EXPECT_EQ(10, v.x);
  EXPECT_EQ(20, v.y);
}

GTEST(sub_assign) {
  auto v = vec2i(1, 2);
  v -= vec2i(2, 4);
  EXPECT_EQ(-1, v.x);
  EXPECT_EQ(-2, v.y);
}

GTEST(times) {
  const auto v = vec2i(1, 2) * 2;
  EXPECT_EQ(2, v.x);
  EXPECT_EQ(4, v.y);
}

GTEST(times_assign) {
  auto v = vec2i(1, 2);
  v *= 2;
  EXPECT_EQ(2, v.x);
  EXPECT_EQ(4, v.y);
}

GTEST(div) {
  const auto v = vec2i(2, 4) / 2;
  EXPECT_EQ(1, v.x);
  EXPECT_EQ(2, v.y);
}

GTEST(div_assign) {
  auto v = vec2i(2, 4);
  v /= 2;
  EXPECT_EQ(1, v.x);
  EXPECT_EQ(2, v.y);
}

GTEST(length_squared) {
  const auto v = vec2i(1, 2).GetLengthSquared();
  EXPECT_EQ(5, v);
}

GTEST(length) {
  const auto v = vec2i(0, 3).GetLength();
  EXPECT_EQ(3, v);
}

GTEST(get_normalized) {
  const auto v = vec2i(0, 3).GetNormalized();
  EXPECT_EQ(0, v.x);
  EXPECT_EQ(1, v.y);
}

GTEST(normalize) {
  auto v = vec2i(0, 3);
  v.Normalize();
  EXPECT_EQ(0, v.x);
  EXPECT_EQ(1, v.y);
}

GTEST(equal) {
  EXPECT_TRUE(vec2i(1, 2) == vec2i(1, 2));
  EXPECT_FALSE(vec2i(1, 2) == vec2i(2, 1));
}

GTEST(dot) {
  const auto r = dot(vec2i(1, 2), vec2i(3, 4));
  EXPECT_EQ(11, r);
}
