#include "gtest/gtest.h"
#include "gmock/gmock-generated-matchers.h"

#include "spacetyper/mat4.h"

#define GTEST(X) TEST(mat4, X)
using namespace testing;

GTEST(colmajor) {
  const auto m = mat4i::FromColMajor(0, 1, 2, 3,
                                     4, 5, 6, 7,
                                     8 , 9 ,10 , 11,
                                     12, 13, 14, 15);
  EXPECT_EQ(vec4i(0, 1, 2, 3), m.GetColumn(0));
  EXPECT_EQ(vec4i(4, 5, 6, 7), m.GetColumn(1));
  EXPECT_EQ(vec4i(8, 9, 10, 11), m.GetColumn(2));
  EXPECT_EQ(vec4i(12, 13, 14, 15), m.GetColumn(3));
}

GTEST(rowmajor) {
  const auto m = mat4i::FromRowMajor(0, 1, 2, 3,
                                     4, 5, 6, 7,
                                     8 , 9 ,10 , 11,
                                     12, 13, 14, 15);
  EXPECT_EQ(vec4i(0, 4, 8, 12), m.GetColumn(0));
  EXPECT_EQ(vec4i(1, 5, 9, 13), m.GetColumn(1));
  EXPECT_EQ(vec4i(2, 6, 10, 14), m.GetColumn(2));
  EXPECT_EQ(vec4i(3, 7, 11, 15), m.GetColumn(3));
}

GTEST(rowmajor_row) {
  const auto m = mat4i::FromRowMajor(0, 1, 2, 3,
                                     4, 5, 6, 7,
                                     8 , 9 ,10 , 11,
                                     12, 13, 14, 15);
  EXPECT_EQ(vec4i(0, 1, 2, 3), m.GetRow(0));
  EXPECT_EQ(vec4i(4, 5, 6, 7), m.GetRow(1));
  EXPECT_EQ(vec4i(8, 9, 10, 11), m.GetRow(2));
  EXPECT_EQ(vec4i(12, 13, 14, 15), m.GetRow(3));
}

GTEST(identity) {
  const auto m = mat4i::FromRowMajor(1, 0, 0, 0,
                                     0, 1, 0, 0,
                                     0, 0, 1, 0,
                                     0, 0, 0, 1);
  const auto i = mat4i::Identity();
  EXPECT_EQ(i.GetColumn(0), m.GetColumn(0));
  EXPECT_EQ(i.GetColumn(1), m.GetColumn(1));
  EXPECT_EQ(i.GetColumn(2), m.GetColumn(2));
  EXPECT_EQ(i.GetColumn(3), m.GetColumn(3));
}

GTEST(index) {
  const auto m = mat4i::FromRowMajor(0, 1, 2, 3,
                                     4, 5, 6, 7,
                                     8 , 9 ,10 , 11,
                                     12, 13, 14, 15);
  EXPECT_EQ(3, m(0, 3));
  EXPECT_EQ(15, m(3, 3));
}

GTEST(major) {
  const auto m = mat4i::FromRowMajor(0, 1, 2, 3,
                                     4, 5, 6, 7,
                                     8 , 9 ,10 , 11,
                                     12, 13, 14, 15).GetMajor();
  EXPECT_EQ(vec4i(0, 5, 10, 15), m);
}


GTEST(transposed) {
  const auto m = mat4i::FromRowMajor(0, 1, 2, 3,
                                     4, 5, 6, 7,
                                     8 , 9 ,10 , 11,
                                     12, 13, 14, 15)
                                     .GetTransposed();
  EXPECT_EQ(mat4i::FromColMajor(0, 1, 2, 3,
                                4, 5, 6, 7,
                                8, 9, 10, 11,
                                12, 13, 14, 15), m);
}

GTEST(multiply) {
  const auto m = mat4i::FromRowMajor(0, 1, 2, 3,
                                     4, 5, 6, 7,
                                     8 , 9 ,10 , 11,
                                     12, 13, 14, 15)
      *
      mat4i::FromRowMajor(16, 17, 18, 19,
                          20, 21, 22, 23,
                          24, 25, 26, 27,
                          28, 29, 30, 31);
  EXPECT_EQ(mat4i::FromRowMajor(0, 1, 2, 3,
                                4, 5, 6, 7,
                                8, 9, 10, 11,
                                12, 13, 14, 15), m);
}

