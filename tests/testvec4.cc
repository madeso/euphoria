#include "gtest/gtest.h"
#include "core/vec4.h"

#define GTEST(X) TEST(vec4, X)


GTEST(constructor_same) {
  const auto v = vec4i(42);
  EXPECT_EQ(42, v.x);
  EXPECT_EQ(42, v.y);
  EXPECT_EQ(42, v.z);
  EXPECT_EQ(42, v.w);
}

GTEST(constructor_unique) {
  const auto v = vec4i(1, 2, 3, 4);
  EXPECT_EQ(1, v.x);
  EXPECT_EQ(2, v.y);
  EXPECT_EQ(3, v.z);
  EXPECT_EQ(4, v.w);
}

GTEST(constructor_vec3) {
  const auto v = vec4i(vec3i(1, 2, 3), 4);
  EXPECT_EQ(1, v.x);
  EXPECT_EQ(2, v.y);
  EXPECT_EQ(3, v.z);
  EXPECT_EQ(4, v.w);
}

GTEST(constructor_array) {
  int arr[4] = {1, 2, 3, 4};
  const auto v = vec4i(arr);
  EXPECT_EQ(1, v.x);
  EXPECT_EQ(2, v.y);
  EXPECT_EQ(3, v.z);
  EXPECT_EQ(4, v.w);
}

GTEST(cast_vec3) {
  const auto v = static_cast<vec3i>(vec4i(1, 2, 3, 4));
  EXPECT_EQ(1, v.x);
  EXPECT_EQ(2, v.y);
  EXPECT_EQ(3, v.z);
}

GTEST(componentsum) {
  const auto v = vec4i(1, 2, 3, 4);
  EXPECT_EQ(10, v.GetComponentSum());
}

GTEST(component_multipl) {
  const auto v = ComponentMultiply(vec4i(1, 2, 3, 4), vec4i(4, 3, 2, 1));
  EXPECT_EQ(vec4i(4, 6, 6, 4), v);
}

